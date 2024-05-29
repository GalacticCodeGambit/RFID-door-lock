#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "lol";                  // SSID of your home WiFi
char pass[] = "lol123456789";         // password of your home WiFi
const byte LEDpin = D4;
String LEDstatus = "aus";

unsigned long lastConnectionAttempt = 0;
const unsigned long connectionAttemptInterval = 5000;  // Versuchsintervall für die Verbindung (in Millisekunden)
unsigned long lastSetTime = 0;
const unsigned long resetInterval = 5 * 1000; // Zeit bis LED wieder aus geht

IPAddress server(192, 168, 137, 80);  // the fix IP address of the server
WiFiClient client;

void setup() {
  Serial.begin(115200);
  pinMode(LEDpin, OUTPUT);

  WiFi.begin(ssid, pass);             // connects to the WiFi router
  Serial.println();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to wifi");
  reconnectToServer();                // Verbindung zum Server herstellen
}

void loop() {
  if (!client.connected()) {         // Bei Verbindungsverlusst mit Server neu Verbinden
    if (millis() - lastConnectionAttempt >= connectionAttemptInterval) {
      reconnectToServer();
    }
  } else {
    handleServerCommunication();
  }
  if (LEDstatus == "an" && millis() - lastSetTime >= resetInterval) {  // LED nach resetInterval auf Low stellen
    digitalWrite(LEDpin, LOW);
    LEDstatus = "aus";
    client.print("LED ist aus\r");
  }
  delay(1000);                                         // Kleine Pause für Stabilität
}

void reconnectToServer() {
  Serial.println("Versuche, die Verbindung zum Server wiederherzustellen...");
  /*  if (client.connected()) { //Zum Testen entfernen
      client.stop();
    }*/
  // Verbindung zum Server herstellen
  if (client.connect(server, 80)) {
    Serial.println("Verbunden mit dem Server.");
  } else {
    Serial.println("Verbindung zum Server fehlgeschlagen.");
  }
  lastConnectionAttempt = millis();    // Aktualisieren des Zeitstempel des letzten Verbindungsversuchs
}

void handleServerCommunication() {
  if (client.available()) {                          // Nachricht vom Server empfangen
    String request = client.readStringUntil('\r');
    if (request.length() > 0) {                      // Nur wenn eine tatsächliche Nachricht empfangen wurde
      Serial.println("Nachricht vom Server: " + request);
      if (request == "LED high") {
        digitalWrite(LEDpin, HIGH);
        LEDstatus = "an";
        lastSetTime = millis();
        client.print("LED ist " + LEDstatus + '\r'); // Antwort an Server senden
      } else if (request == "LED low") {
        digitalWrite(LEDpin, LOW);
        LEDstatus = "aus";
        client.print("LED ist " + LEDstatus + '\r'); // Antwort an Server senden
      }
      request = "";                                  // Request leeren
    }
  }
}
