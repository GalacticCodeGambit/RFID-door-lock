/*  Connects to the home WiFi network
 *  Asks some network parameters
 *  Sends and receives message from the server in every 2 seconds
 *  Communicates: wifi_server_01.ino
 */ 
#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "lol";                  // SSID of your home WiFi
char pass[] = "lol123456789";         // password of your home WiFi

unsigned long askTimer = 0;

IPAddress server(192,168,137,80);     // the fix IP address of the server
WiFiClient client;

void setup() {
  Serial.begin(115200);               // only for debug
  WiFi.begin(ssid, pass);             // connects to the WiFi router
  Serial.println();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to wifi");
  Serial.print("Status: "); Serial.println(WiFi.status());    // Network parameters
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());//*/
}

void loop () {
  client.connect(server, 80);                          // Connection to the server
  Serial.println(".");
  client.println("Hello server! Are you sleeping?\r"); // sends the message to the server
  String answer = client.readStringUntil('\r');        // receives the answer from the sever
  Serial.println("from server: " + answer);
  client.flush();
  delay(2000);                                         // client will trigger the communication after two seconds
}
