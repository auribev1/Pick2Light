#include <ESP8266WiFi.h>

//SSID of your network
char ssid[] = "Alejo";
//password of your WPA Network
char pass[] = "alejouribe";
long rssi;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP8266 WiFi Signal Strength Checker");
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  initWiFi();
}

void loop () {
  SignalStrenght();
  delay(1000);
}

void SignalStrenght (){
  rssi = WiFi.RSSI();
  Serial.print("RSSI: ");
  Serial.println(rssi);
  if (rssi<-70 || WiFi.status() != WL_CONNECTED){
    WiFi.disconnect();
    delay(10);
    initWiFi();      
      }
}
void initWiFi (){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}
