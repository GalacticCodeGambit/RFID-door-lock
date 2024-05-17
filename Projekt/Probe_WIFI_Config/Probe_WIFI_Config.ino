
#include <ESP8266WiFi.h>      //Einbinden der WiFi-Bibliothek ESP8266
#include <ESP8266mDNS.h>  //So kann man den ESP auch über den Hostname im Netz finden
#include <ESP8266WebServer.h>
//#include <WiFi.h>
#include <SPI.h>

const char* ssid = "bastian";     //hier muss die SSID stehen
const char* passwort = "hallo123";    //hier muss das Passwort stehen

ESP8266WebServer server(80);  //Port 80

IPAddress ipnow(192, 168, 0, 0);

int i = 0;

// Set your Static IP address
IPAddress IP(192, 168, 1, 184);
// Set your Gateway IP address
IPAddress gate(192, 168, 1, 1);

IPAddress sub(255, 255, 0, 0);
IPAddress priDNS(8, 8, 8, 8);   //optional
IPAddress secDNS(8, 8, 4, 4); //optional



void setup() {
  Serial.begin(115200);
  Serial.print("\n\nVerbindungsaufbau zu   ");
  Serial.println(ssid);
  WiFi.begin(ssid, passwort); //WiFi-Verbindung herstellen
  //warten bis Verbindung steht
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.print("\nVerbindung hergestellt.\nLokale IP: ");
  Serial.println(WiFi.localIP());

  WiFi.config(IP, gate, sub, priDNS, secDNS);     //corect way
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.print("\nVerbindung hergestellt.\nLokale IP: ");
  Serial.println(WiFi.localIP());
  //  server.on(  "/", HTTP_GET  , [] (AsyncWebServerRequest *request){
  //      request->send(  200, "text/plain", "Hallo ...."  );});

  //   server.onNotFound([] (AsyncWebServerRequest *request){
  //      request->send(404, "text/plain", "Seite nicht vorhanden!");});

  //   server.begin();  //Den Server starten mit der Methode begin()

}
void loop() {
  /*                          //funktioniert nicht
    i++;
    delay(1000);
    WiFi.config((192, 168, 1, 184 + i), gate, sub, priDNS, secDNS);
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
    }
    Serial.print("\nVerbindung hergestellt.\nLokale IP: ");
    Serial.println(WiFi.localIP());
  */
}
