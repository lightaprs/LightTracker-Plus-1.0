#include <WiFi.h>
#include <WiFiClient.h>
#include <APRS-IS.h>

//*********WI-FI Settings**********/
//const char* ssid = "MODEM_SSID";
//const char* password = "SSID_PASSWORD";

const char* ssid = "CHANGE_ME";
const char* password = "CHANGE_ME";

void setup() {
  Serial.begin(115200);
  delay(7000);
  Serial.println(F("APRS-IS Test Started..."));
  setupWiFi();

}

void loop() {
    delay(100);
}

void setupWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("[DONE]");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
}
