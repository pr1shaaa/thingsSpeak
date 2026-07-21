# IoT-network-using-ThingSpeak-and-ESP32

_This tutorial and code are based on the official MathWorks documentation with minor modifications.  
Original guide: [Wireless Network Signal Strength with ESP32 with Arduino IDE](https://www.mathworks.com/help/thingspeak/measure-arduino-wifi-signal-strength-with-esp32.html)._

Before running the code below, please follow the steps to register your ESP32 with the school's wifi network.

## Wi-Fi Connection
### First Glimpse of Wi-Fi Connection
The Wi-Fi functionality of the ESP32 enables it to measure the signal strength of the wireless network it connects to. In this step, we will implement a simple connection to test this baseline functionality.
```cpp
// source code: connect_wifi.ino
#include <WiFi.h>

char* ssid = "WIFINAME";
const char* password = "your_password";

void setup() {
  // Initialize serial communication at 115200 baud
  Serial.begin(115200);
  delay(1000); 

  // Set Wi-Fi 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);

  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); 
    Serial.print(".");
  }
  Serial.println("\nConnected successfully!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    long rssi = WiFi.RSSI(); // Read Received Signal Strength Indicator (RSSI)
    
    Serial.print("Signal strength : ");
    Serial.print(rssi);
    Serial.println(" dBm");
  } else {
    Serial.println("Wi-Fi disconnected!");
  }
  delay(2000); 
}
```
### Manual Trigger & LED Indicators
Now that the baseline connection is verified, let's step up the complexity. Instead of looping automatically, we will use the onboard **BOOT** button to manually trigger measurements and use the **built-in LED** as an indicator.

To make the code more organized, core operations are wrapped up into custom functions as shown in the original MathWorks document to simplify the `setup()` and `loop()` structures.

```cpp
// ==========================================
// FUNCTIONS FROM THE ORIGINAL MATHWORKS DOC
// ==========================================

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

// Take measurements of the Wi-Fi strength and return the average result.
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
```

In the following code, the ESP32 measures and prints the RSSI value each time the button is pressed. The built-in LED blinks twice to provide visual feedback for the measurement execution.

```cpp
// source code: Wi-Fi-button.ino
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
        wifiStrength = getStrength(numberPoints); // Read Received Signal Strength Indicator (RSSI)
        Serial.print("Signal strength : ");
        Serial.print(wifiStrength);
        Serial.println(" dBm");
        }
}
```

## Sending Data to ThingSpeak

ThingSpeak is an IoT analytics platform service that enables us to store, visualize, and analyze data in the cloud.

To interface your ESP32 with the platform, follow the steps outlined in the official MathWorks documentation:
1. Sign up for a free account at [ThingSpeak](https://thingspeak.com/).
2. Create a new **Channel** and verify that both `Field 1` (for RSSI) and `Field 2` (for your measurement counter) are enabled.
3. Copy your unique **Write API Key** from the *API Keys* tab and paste it into the secrets.h file.

```cpp
// source code: ESP32-Thingspeak.ino

// ==========================================
// FROM THE ORIGINAL MATHWORKS DOC 
// ==========================================
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
```

The raw HTTP POST request logic is wrapped up into a single custom function: `httpRequest()`. 
This function accepts your RSSI data and counter, manually builds the standard web package headers line-by-line, and uploads them to ThingSpeak.

To simplify code maintenance, MathWorks provides the official `ThingSpeak.h` package.
It hides the raw HTTP headers and replaces them with clean, easy-to-use function calls.
> [!NOTE]
> To compile, install the ThingSpeak library via the Arduino IDE Library Manager.

```cpp
// source code: ESP32-Thingspeak_V2.ino
#include <WiFi.h>
#include <ThingSpeak.h>
#include "secrets.h"

#define buttonPin 0 
#define LEDPin    2

WiFiClient client;

const unsigned long postingInterval = 15L * 1000L; by ThingSpeak

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
```
