#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "lol";                  // SSID of your home WiFi
char pass[] = "lol123456789";         // password of your home WiFi
const byte LEDpin = D4;               
String LEDstatus = "aus";

unsigned long lastConnectionAttempt = 0;
const unsigned long connectionAttemptInterval = 5000;  // Versuchsintervall für die Verbindung (in Millisekunden)

IPAddress server(192, 168, 137, 80);  // the fix IP address of the server
WiFiClient client;

void setup() {
  pinMode(LEDpin, OUTPUT);            // Setze den LED-Pin als Ausgang
  Serial.begin(115200);               // only for debug
  WiFi.begin(ssid, pass);             // connects to the WiFi router
  Serial.println();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to wifi");
  // Verbindung zum Server herstellen
  reconnectToServer();
}

void loop() {
  if (!client.connected()) {  // Bei Verbindungsverlusst mit Server neu Verbinden 
    if (millis() - lastConnectionAttempt >= connectionAttemptInterval) {
      reconnectToServer();
    }
  } else {
    // Nachricht vom Server empfangen
    if (client.available()) {
      String request = client.readStringUntil('\r');
      if (request.length() > 0) {                       // Nur wenn eine tatsächliche Nachricht empfangen wurde
        Serial.println("Nachricht vom Server: " + request);
        if (request == "LED high") {
          digitalWrite(LEDpin, HIGH);
          LEDstatus = "an";
          client.print("LED ist " + LEDstatus + '\r');  // Antwort an Server senden
          request = "";                                 // Request leeren
        } else if (request == "LED low") {
          digitalWrite(LEDpin, LOW);
          LEDstatus = "aus";
          client.print("LED ist " + LEDstatus + '\r');  // Antwort an Server senden
          request = "";                                 // Request leeren
        }
      }
    }
  }
  delay(1000);                                          // Kleine Pause für Stabilität
}

void reconnectToServer() {
  Serial.println("Versuche, die Verbindung zum Server wiederherzustellen...");
  // Wenn eine vorhandene Verbindung vorhanden ist, beende diese
  if (client.connected()) {
    client.stop();
  }
  // Verbindung zum Server herstellen
  if (client.connect(server, 80)) {
    Serial.println("Verbunden mit dem Server.");
  } else {
    Serial.println("Verbindung zum Server fehlgeschlagen.");
  }
  lastConnectionAttempt = millis();    // Aktualisieren des Zeitstempel des letzten Verbindungsversuchs
}
