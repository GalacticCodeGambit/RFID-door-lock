#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = "lol";
const char* password = "lol123456789";
const char* host = "192.168.137.169"; // IP-Adresse des ESP8266 (1)
const uint8_t port = 80;


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

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // LED aus (HIGH ist aus)
}

void loop() {
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("Connection to host failed");
    delay(1000);
    return;
  }

  // Warte auf die Antwort vom Server
  while (client.connected()) {
    while (client.available()) {
      String status = client.readStringUntil('\n');
      Serial.println("Status received: " + status);
      if (status == "Entriegelt") {
        digitalWrite(LED_BUILTIN, LOW); // LED ein (LOW ist ein)
        Serial.println("LED turned on");
        delay(5000); // Zeit für entriegelten Zustand
        digitalWrite(LED_BUILTIN, HIGH); // LED aus (HIGH ist aus)
        Serial.println("LED turned off");
      } else if (status == "Abgelehnt") {
        digitalWrite(LED_BUILTIN, HIGH); // LED aus (HIGH ist aus)
        Serial.println("LED remains off");
      }
    }
  }

  delay(10000); // Wartezeit zwischen den Abfragen
}
