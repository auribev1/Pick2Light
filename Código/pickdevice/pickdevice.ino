//http://arduino.esp8266.com/stable/package_esp8266com_index.json
/**
   BasicHTTPClient.ino
    Created on: 24.05.2015
*/

#include <Arduino.h>
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

#include <TM1637Display.h>

const int CLK = D4;
const int DIO = D3;
bool sending_flag = 0;

// identificador del dispositivo
String id = "PREBEL01";

String quantitiesEndPoint = "http://104.131.69.66/quantities";   // señal para cantidad
String signalEndPoint = String("http://104.131.69.66/signal?id=") + id + "&cantidad=";

int refreshRate = 3000;


int rootCantidad = 0;
int rootEjecutadoPicking = 0;
int rootLlego = 0;
int devices = 3;

// Current time
// Previous time
unsigned long lastMillis = millis();

int estado = 1;
int qty = 0;

#define signal_button D6
#define plus_button D5
#define less_button D7

TM1637Display display(CLK, DIO); //set up the 4-Digit Display.


ESP8266WiFiMulti WiFiMulti;

// Aqui van las credenciales para el nivel de red
//SSID of your network
char ssid[] = "Alejo";
//password of your WPA Network
char pass[] = "alejouribe";
long rssi;
//Threshold de la señal de WIFI
float threshold = -90;


void setup() {
  pinMode(signal_button, INPUT);
  pinMode(plus_button, INPUT_PULLUP);
  pinMode(less_button, INPUT_PULLUP);
  Serial.begin(115200);

  display.showNumberDec(0);
  delay(1000);
  display.clear();

  //espera 4 seg por un output serial que no se cual sea
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  WiFi.mode(WIFI_STA);//sta es para conexion por access point
  //WiFiMulti.addAP("element4plus", "kalleymovil");
  //WiFiMulti.addAP("proyecto-wms", "servibarras");//(nombre de red, contraseña)
  WiFiMulti.addAP(ssid, pass);
  
  display.setBrightness(0x0a); //set the diplay to maximum brightness
  display.clear();
  Serial.println("End setup");

  display.showNumberDec(8888);
  delay(1000);
  display.clear();
}

void add_value() {
  rootCantidad ++;
  delay(300);
}

void substract_value() {
  rootCantidad --;
  delay(300);

}

void get_pedidos() {
  //recibe el json actual de ordenes
  // wait for WiFi connection
  WiFiClient client;
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  if (http.begin(client, quantitiesEndPoint)) {  // HTTP
    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        const size_t capacity = 2000;
        Serial.println("capacity");
        Serial.println(JSON_ARRAY_SIZE(1));
        Serial.println(JSON_OBJECT_SIZE(1));
        DynamicJsonDocument doc(capacity);
        Serial.println(payload);
        deserializeJson(doc, payload);

        JsonObject root_0 = doc[id];
        Serial.println(root_0);
        rootCantidad = root_0["Cantidad"];
        rootEjecutadoPicking = root_0["Ejecutado_picking"];
        rootLlego = root_0["Llego"];

        Serial.print("rootCantidad = ");
        Serial.println(rootCantidad);
        Serial.print("rootEjecutadoPicking = ");
        Serial.println(rootEjecutadoPicking);
        Serial.print("rootLlego = ");
        Serial.println(rootLlego);

      }
    }
    else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
  }
}


void send_signal() {
  //señal de que se hundio el boton
  // wait for WiFi connection
  WiFiClient client;
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  Serial.print(signalEndPoint + rootCantidad);
  if (http.begin(client, signalEndPoint + rootCantidad)) {  // HTTP
    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        Serial.println(payload);

      }
    }
    else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
  }
}

void SignalStrenght (){
  rssi = WiFi.RSSI();
  Serial.print("RSSI: ");
  Serial.println(rssi);
  if (rssi<threshold || WiFi.status() != WL_CONNECTED){
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

void loop(void) {
  SignalStrenght();
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    bool buttonState = digitalRead(signal_button);
    bool plusState = digitalRead(plus_button);
    bool lessState = digitalRead(less_button);

    switch (estado) {

      // Inicial
      case 1:
        //  LLendo hacia la siguiente posicion: pregunte hasta que llegue a la posicion

        if (millis() - lastMillis > refreshRate) {
          lastMillis = millis();
          get_pedidos();
          if (rootLlego == 0) {
            Serial.print("LLendo hacia la siguiente posicion: pregunte hasta que llegue a la posicion / estado = ");
          }
          else if ((rootLlego == 1) && (rootCantidad == 0)) {
            Serial.print("A este dispositivo no le toca en esta posicion: pregunte hasta que llegue a la posicion / estado = ");
          }
          else if ((rootLlego == 1) && (rootCantidad > 0) && (rootEjecutadoPicking == 1)) {
            Serial.print("ya se ejecuto picking: esperando confirmación de operario / estado = ");
          }
          Serial.println(estado);
        }
        if ((rootLlego == 1) && (rootCantidad > 0) && (rootEjecutadoPicking == 0) ) {
          estado = 2;
        }
        break;

      case 2:
        // Llega a posicion, no ha confirmado picking aun
        display.showNumberDec(rootCantidad);
        //Si se presiona el boton
        if (millis() - lastMillis > refreshRate) {
          lastMillis = millis();
          Serial.print("Llega a posicion, no ha confirmado picking aun / estado = ");
          Serial.println(estado);
        }
        if (plusState == 1) {
          add_value();
        }
        if (lessState == 1) {
          substract_value();
        }
        if (buttonState == 1) {
          estado = 1;
          display.clear();
          send_signal();
          get_pedidos();
        }
        break;

    }
  }
}
