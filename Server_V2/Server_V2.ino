#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "lol";                      // SSID of your home WiFi
char pass[] = "lol123456789";             // password of your home WiFi
WiFiServer server(80);
WiFiClient clients[5];                    // Array für bis zu 5 Clients

IPAddress ip(192, 168, 137, 80);          // IP address of the server
IPAddress gateway(0, 0, 0, 0);            // gateway of your network
IPAddress subnet(255, 255, 255, 0);       // subnet mask of your network

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
  Serial.println("\nConnected to wifi\nServer started");
}

void loop () {
  // Neue Clients akzeptieren
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

  // Nachrichten von Clients verarbeiten und an den jeweiligen Client antworten
  for (int i = 0; i < 5; i++) {
    if (clients[i] && clients[i].connected()) {
      if (clients[i].available()) {
        String request = clients[i].readStringUntil('\r');
        Serial.print("Client ");
        Serial.print(i);
        Serial.print(" says: ");
        Serial.println(request);
        clients[i].flush();

        // Nachricht zurück an den Client senden
        if (request == "Requesting LED status") {
          clients[i].print("LED high\r");  // Senden der LED-Status-Anforderung
        }
      }
    }
  }
}
