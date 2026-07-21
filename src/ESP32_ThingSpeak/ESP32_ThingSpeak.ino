#include <WiFi.h>
#include "secrets.h"

#define buttonPin 0 
#define LEDPin 2




// Constants
const unsigned long postingInterval = 15L * 1000L;

// Global variables
unsigned long lastConnectionTime = 0;
int measurementNumber = 0;

void setup(){
  Serial.begin(115200);
  pinMode(buttonPin,INPUT);
  pinMode(LEDPin, OUTPUT);
  connectWiFi();
}


void loop(){
  const int numberPoints = 7;
  float wifiStrength;
  // In each loop, make sure there is an Internet connection.
  if (WiFi.status() != WL_CONNECTED) { 
    connectWiFi();
    }

  // If a button press is detected, write the data to ThingSpeak.
  if (digitalRead(buttonPin) == LOW){
    if (millis() - lastConnectionTime > postingInterval) {
      blinkX(2,250); // Verify the button press.
      wifiStrength = getStrength(numberPoints); 
      httpRequest(wifiStrength, measurementNumber);
      blinkX(measurementNumber,200);  // Verify that the httpRequest is complete.
      measurementNumber++;
      }
        
    }
}


void connectWiFi(){
    while (WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password);
        delay(3000);
    }
    // Display a notification that the connection is successful. 
    Serial.println("Connected");
    blinkX(5,50);  
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

void httpRequest(float field1Data, int field2Data) {
  WiFiClient client;
  if (!client.connect(server, 80)){
    Serial.println("Connection failed");
    lastConnectionTime = millis();
    client.stop();
    return;     
    }
  else{
    // Create data string to send to ThingSpeak.
    String data = "field1=" + String(field1Data) + "&field2=" + String(field2Data); //shows how to include additional field data in http post
    // POST data to ThingSpeak.
    if (client.connect(server, 80)) {    
      client.println("POST /update HTTP/1.1");
      client.println("Host: api.thingspeak.com");
      client.println("Connection: close");
      client.println("User-Agent: ESP32WiFi/1.1");
      client.println("X-THINGSPEAKAPIKEY: "+writeAPIKey);
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.print(data.length());
      client.print("\n\n");
      client.print(data);
      Serial.println("RSSI = " + String(field1Data));
      lastConnectionTime = millis();   
      delay(250);
      }
    }
    client.stop();
}



