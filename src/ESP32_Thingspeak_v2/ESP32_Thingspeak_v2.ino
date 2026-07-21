#include <WiFi.h>
#include <ThingSpeak.h> 
#include "secrets.h"

#define buttonPin 0 
#define LEDPin    2

WiFiClient client;


const unsigned long postingInterval = 15L * 1000L; 

unsigned long lastConnectionTime = 0;
int measurementNumber = 0;

void setup(){
  Serial.begin(115200);
  
  pinMode(buttonPin, INPUT_PULLUP); 
  pinMode(LEDPin, OUTPUT);
  
  connectWiFi();
    ThingSpeak.begin(client); 
}

void loop(){
  const int numberPoints = 7;
  float wifiStrength;

  if (WiFi.status() != WL_CONNECTED) { 
    connectWiFi();
  }

  if (digitalRead(buttonPin) == LOW){
    if (millis() - lastConnectionTime > postingInterval) {
      blinkX(2, 250); 
      
      wifiStrength = getStrength(numberPoints); 
      httpRequest(wifiStrength, measurementNumber);
      
      blinkX(measurementNumber, 200);
      measurementNumber++;
    }
  }
}


void httpRequest(float field1Data, int field2Data) {

  ThingSpeak.setField(1, field1Data);
  ThingSpeak.setField(2, field2Data);
  
  int httpResponseCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
  if (httpResponseCode == 200) {
    Serial.print("RSSI = ");
    Serial.print(field1Data);
  } else {
    Serial.print("Problem updating channel. HTTP error code: ");
    Serial.println(httpResponseCode);
  }
  
  lastConnectionTime = millis(); 
}


void connectWiFi(){
    while (WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password);
        delay(3000);
    }
    Serial.println("Connected");
    blinkX(5, 100);  
}

void blinkX(int numTimes, int delayTime){ 
    for (int g = 0; g < numTimes; g++){
        digitalWrite(LEDPin, HIGH);  
        delay(delayTime);
        digitalWrite(LEDPin, LOW);
        delay(delayTime);   
    }
}

int getStrength(int points){
  long rssi = 0;
  for (int i = 0; i < points; i++){
    rssi += WiFi.RSSI();
    delay(20);
  }
  return rssi / points;
}