#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "lol";                      // SSID of your home WiFi
char pass[] = "lol123456789";             // password of your home WiFi
WiFiServer server(80);
WiFiClient clients[5];                    // Array für bis zu 5 Clients

IPAddress ip(192, 168, 137, 80);          // IP address of the server
IPAddress gateway(0, 0, 0, 0);            // gateway of your network
IPAddress subnet(255, 255, 255, 0);       // subnet mask of your network

bool sendLedHigh = false;                 // Variable to initiate LED high request
bool sendLedLow = false;                  // Variable to initiate LED low request
String LEDstatus = "low";                 // Current status of LED

unsigned long lastSendTime = 0;
const unsigned long resendInterval = 3000; // Interval for resending message (3 seconds)
int resendCount = 0;
const int maxResendAttempts = 2;
bool waitingForResponse = false;
String expectedResponse = "";

void setup() {
  Serial.begin(115200);
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
  // Accept new clients
  if (server.hasClient()) {
    for (int i = 0; i < 5; i++) {
      // Freier Slot finden
      if (!clients[i] || !clients[i].connected()) {
        if (clients[i]) clients[i].stop();
        clients[i] = server.available();
        Serial.print("New client connected, ID: ");
        Serial.println(i);
        break;
      }
    }
  }

  // If sendLedHigh is true, send "LED high" to all connected clients
  if (sendLedHigh) {
    sendLedHighToClients();
    sendLedHigh = false;   // Reset the variable to avoid resending immediately
    waitingForResponse = true;
    lastSendTime = millis();
    expectedResponse = "LED ist an";
  }

  // If sendLedLow is true, send "LED low" to all connected clients
  if (sendLedLow) {
    sendLedLowToClients();
    sendLedLow = false;    // Reset the variable to avoid resending immediately
    waitingForResponse = true;
    lastSendTime = millis();
    expectedResponse = "LED ist aus";
  }

  // Process responses from clients
  if (waitingForResponse) {
    bool allClientsResponded = true;
    for (int i = 0; i < 5; i++) {
      if (clients[i] && clients[i].connected()) {
        if (clients[i].available()) {
          String response = clients[i].readStringUntil('\r');
          Serial.print("Client ");
          Serial.print(i);
          Serial.print(" says: ");
          Serial.println(response);
          clients[i].flush();

          if (response == expectedResponse) {
            Serial.print("Client ");
            Serial.print(i);
            Serial.println(" confirmed LED status");
          } else {
            allClientsResponded = false;
          }
        } else {
          allClientsResponded = false;
        }
      }
    }

    if (allClientsResponded) {
      LEDstatus = (expectedResponse == "LED ist an") ? "high" : "low";
      Serial.println("All clients confirmed LED status");
      waitingForResponse = false;   // Stop waiting for responses
      resendCount = 0;              // Reset resend count after successful responses
    } else if (millis() - lastSendTime >= resendInterval) {
      if (resendCount < maxResendAttempts) {
        Serial.println("Resending LED status to clients...");
        if (expectedResponse == "LED ist an") {
          sendLedHighToClients();
        } else {
          sendLedLowToClients();
        }
        lastSendTime = millis();
        resendCount++;
      } else {
        Serial.println("Client antwortet nicht");
        waitingForResponse = false; // Stop waiting after max attempts
        resendCount = 0;
      }
    }
  }
}

void sendLedHighToClients() {
  for (int i = 0; i < 5; i++) {
    if (clients[i] && clients[i].connected()) {
      clients[i].print("LED high\r");
    }
  }
}

void sendLedLowToClients() {
  for (int i = 0; i < 5; i++) {
    if (clients[i] && clients[i].connected()) {
      clients[i].print("LED low\r");
    }
  }
}
