/*  Connects to the home WiFi network
    Asks some network parameters
    Starts WiFi server with fix IP and listens
    Receives and sends messages to the client
    Communicates: wifi_client_01.ino
*/
#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "lol";                      // SSID of your home WiFi
char pass[] = "lol123456789";             // password of your home WiFi
WiFiServer server(80);
WiFiClient clients[5];                    // Array für bis zu 5 Clients
String messages[10];                      // Warteschlange für bis zu 10 Nachrichten
int messageCount = 0;

IPAddress ip(192, 168, 137, 80);       // IP address of the server
IPAddress gateway(0, 0, 0, 0);         // gateway of your network
IPAddress subnet(255, 255, 255, 0);    // subnet mask of your network

void setup() {
  Serial.begin(115200);                   // only for debug
  WiFi.config(ip, gateway, subnet);       // forces to use the fix IP
  WiFi.begin(ssid, pass);                 // connects to the WiFi router
  Serial.println();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  server.begin();                         // starts the server
  Serial.println("Connected to wifi\nServer started");
  Serial.print("Status: "); Serial.println(WiFi.status());  // some parameters from the network
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());
  Serial.print("Networks: "); Serial.println(WiFi.scanNetworks());//*/
}

void loop () {
  // Neue Clients akzeptieren
  if (server.hasClient()) {
    for (int i = 0; i < 5; i++) {
      // Freier Slot gefunden
      if (!clients[i] || !clients[i].connected()) {
        if (clients[i]) clients[i].stop();
        clients[i] = server.available();
        Serial.print("New client connected, ID: ");
        Serial.println(i);
        break;
      }
    }
  }

  // Nachrichten von Clients verarbeiten und in die Warteschlange legen
  for (int i = 0; i < 5; i++) {
    if (clients[i] && clients[i].connected()) {
      if (clients[i].available()) {
        String request = clients[i].readStringUntil('\r');
        Serial.print("Client ");
        Serial.print(i);
        Serial.print(" says: ");
        Serial.println(request);
        clients[i].flush();

        // Nachricht zur Warteschlange hinzufügen
        if (messageCount < 10) {
          messages[messageCount++] = request;
        }
      }
    }
  }

  // Nachrichten aus der Warteschlange an alle Clients senden
  for (int m = 0; m < messageCount; m++) {
    for (int j = 0; j < 5; j++) {
      if (clients[j] && clients[j].connected()) {
        clients[j].println("Nachricht vom Server: " + messages[m]);
      }
    }
  }

  // Warteschlange leeren
  messageCount = 0;
}

/*
  WiFiClient client = server.available();
  if (client) {
  if (client.connected()) {
    Serial.println(".");
    String request = client.readStringUntil('\r');      // receives the message from the client
    Serial.print("From client: "); Serial.println(request);
    client.flush();
    client.println("Hi client! No, I am listening.\r"); // sends the answer to the client
  }
  client.stop();                                        // tarminates the connection with the client
  }*/
