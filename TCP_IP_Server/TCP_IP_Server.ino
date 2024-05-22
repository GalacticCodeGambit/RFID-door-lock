#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "lol";
const char* password = "lol123456789";

ESP8266WebServer server(80);

String statusMessage = "Abgelehnt"; // Standardmäßig abgelehnt

void handleRoot() {
  String message = "Checkt Karte..."; // Beispieltext
  server.send(200, "text/plain", message);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  
  if (client) {
    Serial.println("Client connected");

    // RFID-Code lesen und überprüfen (Beispielcode)
    String rfidCode = "123456"; // Hier der tatsächlich gelesene RFID-Code
    if (rfidCode == "123456") { // Hier die Validierungslogik
      statusMessage = "Entriegelt";
    } else {
      statusMessage = "Abgelehnt";
    }

    // Sende den Status an den Client
    client.print(statusMessage);
    Serial.println("Status sent: " + statusMessage);

    delay(5000); // Zeit für entriegelten Zustand
    statusMessage = "Abgelehnt"; // Zurücksetzen des Status

    client.stop();
    Serial.println("Client disconnected");
  }
}
