#include <SPI.h>
#include <LoRa.h>

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 10
#define moistureSensorPin 4 // GPIO4 podłączony do AO
#define relayPin 38 // GPIO38 podłączony do przekaźnika
#define numReadings 10 // Liczba pomiarów do uśredniania

unsigned long lastSendTime = 0;
unsigned long lastRequestTime = 0;
unsigned long interval = 600000;  
unsigned long requestCooldown = 5000; 

int threshold = 50; 

void setup() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); 

  Serial.begin(115200);
  while (!Serial);

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  
  if (!LoRa.begin(433E6)) { 
    Serial.println("Inicjalizacja LoRa nie powiodła się.");
    while (1);
  }
  
  Serial.println("Nadajnik LoRa gotowy.");
}

void loop() {
  unsigned long currentMillis = millis();

  int packetSize = LoRa.parsePacket();
  if (packetSize > 0) { 
    int packetHeader = LoRa.read();  
    
    if (packetHeader == 0x01 && (currentMillis - lastRequestTime >= requestCooldown)) {
      performMeasurement();
      lastRequestTime = currentMillis; 
    } 
    else if (packetHeader == 0x02) {
      if (LoRa.available()) { 
        int receivedThreshold = LoRa.read(); 
        if (receivedThreshold >= 0 && receivedThreshold <= 100) {
          threshold = receivedThreshold;
          Serial.print("Nowy próg wilgotności ustawiony na: ");
          Serial.println(threshold);
        } else {
          Serial.print("Otrzymano nieprawidłowy próg: ");
          Serial.println(receivedThreshold);
        }
      }
    } else {
      Serial.print("Odebrano nieznaną paczkę o nagłówku: ");
      Serial.println(packetHeader);
    }
  }

  if (currentMillis - lastSendTime >= interval) {
    performMeasurement();
    lastSendTime = currentMillis;
  }
}

void performMeasurement() {
  int sum = 0;

  for (int i = 0; i < numReadings; i++) {
    sum += analogRead(moistureSensorPin);
    delay(50);
  }

  int averageValue = sum / numReadings;

  int moisturePercent = map(averageValue, 4095, 700, 0, 100);

  moisturePercent = constrain(moisturePercent, 0, 100);
  
  int relayState = (moisturePercent < threshold) ? HIGH : LOW;
  digitalWrite(relayPin, relayState);

  sendPacket(moisturePercent, relayState, threshold);
}

void sendPacket(int moisture, int relayState, int threshold) {
  LoRa.beginPacket();
  LoRa.write(0x02);    
  LoRa.write(moisture);  
  LoRa.write(relayState);  
  LoRa.write(threshold);  
  LoRa.endPacket();

  Serial.print("Wysłano dane: Wilgotność: ");
  Serial.print(moisture);
  Serial.print("%, Stan przekaźnika: ");
  Serial.print(relayState);
  Serial.print(", Próg: ");
  Serial.println(threshold);
}
