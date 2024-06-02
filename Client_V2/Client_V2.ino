#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "bastian";                                  // SSID of your WiFi
char pass[] = "Mechatronik1!";                         // Password of your WiFi
const byte LEDpin = D4;
String LEDstatus = "off";

unsigned long currentMillis = 0; 
unsigned long lastConnectionAttempt = 0;
const unsigned long connectionAttemptInterval = 5000; // Connection attempt interval (in milliseconds)

IPAddress server(192, 168, 137, 80);                  // The fix IP address of the server
WiFiClient client;

void setup() {
  Serial.begin(115200);
  pinMode(LEDpin, OUTPUT);

  WiFi.begin(ssid, pass);                             // Connects to the WiFi router
  Serial.println();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to wifi");
  reconnectToServer();                                // Connect to the server
}

void loop() {
  currentMillis = millis();
  if (!client.connected()) {                          // If connection is lost, reconnect to server
    if (currentMillis - lastConnectionAttempt >= connectionAttemptInterval) {
      reconnectToServer();
    }
  } else {
    handleServerCommunication();
  }
  delay(1000);                                        // Small break for stability
}

void reconnectToServer() {
  Serial.println("Attempting to reconnect to the server...");
  // Verbindung zum Server herstellen
  if (client.connect(server, 40)) {
    Serial.println("Connected to the server.");
  } else {
    Serial.println("Connection to server failed.");
  }
  lastConnectionAttempt = millis();
}

void handleServerCommunication() {
  if (client.available()) {                           // Message from server
    String request = client.readStringUntil('\r');

    if (request.length() > 0) {                       // Verify that data has been received
      Serial.println("Nachricht vom Server: " + request);
      if (request == "LED high") {
        digitalWrite(LEDpin, HIGH);
        LEDstatus = "on";
        client.print("LED is " + LEDstatus + '\r');  // Send response to server
      } else if (request == "LED low") {
        digitalWrite(LEDpin, LOW);
        LEDstatus = "off";
        client.print("LED is " + LEDstatus + '\r');
      }
      request = "";
    }
  }
}
