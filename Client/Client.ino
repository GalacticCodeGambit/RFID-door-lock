#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "lol";                  // SSID of your home WiFi
char pass[] = "lol123456789";         // password of your home WiFi

//unsigned long askTimer = 0;

unsigned long lastConnectionAttempt = 0;
const unsigned long connectionAttemptInterval = 5000;  // Versuchsintervall für die Verbindung (in Millisekunden)

IPAddress server(192, 168, 137, 80);  // the fix IP address of the server
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
/*  Serial.print("Status: "); Serial.println(WiFi.status());    // Network parameters
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());//*/
  // Verbindung zum Server herstellen
  reconnectToServer();
}

void loop() {
  if (!client.connected()) {
    // Wenn die Verbindung zum Server unterbrochen ist, versuchen Sie, sie wiederherzustellen
    if (millis() - lastConnectionAttempt >= connectionAttemptInterval) {
      reconnectToServer();
    }
  } else {
    // Nachricht an den Server senden
    client.println("Hello server! Are you sleeping?\r");

    // Antwort vom Server empfangen
    if (client.available()) {
      String answer = client.readStringUntil('\r');
      Serial.println("Antwort vom Server: " + answer);
    }
  }

  // Kleine Pause für Stabilität
  delay(1000);
}

void reconnectToServer() {
  Serial.println("Versuche, die Verbindung zum Server wiederherzustellen...");

  // Wenn eine vorhandene Verbindung vorhanden ist, beenden Sie sie
  if (client.connected()) {
    client.stop();
  }

  // Verbindung zum Server herstellen
  if (client.connect(server, 80)) {
    Serial.println("Verbunden mit dem Server.");
  } else {
    Serial.println("Verbindung zum Server fehlgeschlagen.");
  }

  // Aktualisieren Sie den Zeitstempel des letzten Verbindungsversuchs
  lastConnectionAttempt = millis();
}
