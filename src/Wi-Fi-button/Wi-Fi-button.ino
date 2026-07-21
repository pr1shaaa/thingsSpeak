#include <WiFi.h>
#include "secrets.h"

#define LEDPin 2
#define buttonPin 0 





void setup() {
  
  pinMode(buttonPin,INPUT);
  pinMode(LEDPin, OUTPUT);
  
  // Initialize serial communication at 115200 baud
  Serial.begin(115200);
  delay(1000); 
  connectWiFi();
}

void loop() {
  const int numberPoints = 7;
  float wifiStrength;
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
    }
  if (digitalRead(buttonPin)== LOW){
    blinkX(2,250);
    wifiStrength = getStrength(numberPoints);
    Serial.print("Signal strength : ");
    Serial.print(wifiStrength);
    Serial.println(" dBm");
  }
}




void blinkX(int numTimes, int delayTime){ 
    for (int g=0;g < numTimes;g++){

        // Turn the LED on and wait.
        digitalWrite(LEDPin, HIGH);  
        delay(delayTime);

        // Turn the LED off and wait.
        digitalWrite(LEDPin, LOW);
        delay(delayTime);
        
    }
}
void connectWiFi(){

    while (WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password);
        delay(3000);
    }

    // Display a notification that the connection is successful. 
    Serial.println("Connected");
    blinkX(5,200);  
}

int getStrength(int points){
  long rssi = 0;
  long averageRSSI = 0;
  for (int i=0;i < points;i++){
    rssi += WiFi.RSSI();
    delay(20);
    }
  averageRSSI = rssi/points;
  return averageRSSI;
}




