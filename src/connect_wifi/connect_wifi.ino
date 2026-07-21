#include <WiFi.h>


char* ssid = "WIFINAME";
const char* password = "PPPPPPPPP";

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