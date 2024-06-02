// TCP/IP
#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "lol";                       // SSID of your WiFi
char pass[] = "lol123456789";              // Password of your WiFi
const byte maxClientNumber = 5;
WiFiServer server(80);                     // Server Port
WiFiClient clients[maxClientNumber];

IPAddress ip(192, 168, 137, 80);           // IP address of the server
IPAddress gateway(0, 0, 0, 0);             // Gateway of your network
IPAddress subnet(255, 255, 255, 0);        // Subnet mask of your network

bool sendLedHigh = false;                  // Variable to initiate LED high request
bool sendLedLow = false;                   // Variable to initiate LED low request
String LEDstatus = "low";                  // Current status of LED

unsigned long lastSendTime = 0;
const unsigned long resendInterval = 3000; // Interval for resending message
byte resendCount = 0;
const byte maxResendAttempts = 2;
bool waitingForResponse = false;
String expectedResponse = "";

// RFID
#include <MFRC522.h>
#define SS_PIN  D8
#define RST_PIN  D4

MFRC522 mfrc522(SS_PIN, RST_PIN);          // MFRC522-Instanz erstellen
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

byte authenticationKey[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
byte dataKey[16] = {                       // Setting the data-key
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0xfe, 0x3f
};
const byte dataKeyBlockNumber = 2;         // Location of the data-key in the block
byte readData[18];
byte len = 18;
bool rfidReadyMessageDisplayed = false;    // Variable to track if the RFID ready message was displayed


void setup() {
  Serial.begin(115200);
  WiFi.config(ip, gateway, subnet);        // Forces to use the fix IP
  WiFi.begin(ssid, pass);                  // Connects to the WiFi router
  Serial.println();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  server.begin();                          // Starts the server
  Serial.println("\nConnected to wifi\nServer started");

  // RFID
  SPI.begin();
  mfrc522.PCD_Init();                      // Starting the RFID sensor
  delay(4);
  for (byte i = 0; i < 6; i++)key.keyByte[i] = authenticationKey[i]; //Key festlegen
}

void loop () {
  // Accept new clients
  if (server.hasClient()) {
    for (byte i = 0; i < maxClientNumber; i++) {
      // Freier Slot finden
      if (!clients[i] || !clients[i].connected()) {
        if (clients[i]) {
          clients[i].stop();
        }
        clients[i] = server.available();
        Serial.print("New client connected, ID: ");
        Serial.println(i);
        break;
      }
    }
  }

  // Counts the connected clients
  byte connectedClients = 0;
  for (byte i = 0; i < maxClientNumber; i++) {
    if (clients[i]) {
      if (clients[i].connected()) {
        connectedClients++;
      } else {
        clients[i].stop();
      }
    }
  }

  if (connectedClients != 0) {
    if (!rfidReadyMessageDisplayed) {
      Serial.println("\nRFID-Reader ready to read...\n\n");
      rfidReadyMessageDisplayed = true;
    }
    // Starts reading as soon as a card is placed
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      readRFIDcard();
    }

    // Send "LED high" to all connected clients
    if (sendLedHigh) {
      sendLedHighToClients();
      sendLedHigh = false;
      waitingForResponse = true;
      lastSendTime = millis();
      expectedResponse = "LED is on";
    }

    // Send "LED low" to all connected clients
    if (sendLedLow) {
      sendLedLowToClients();
      sendLedLow = false;
      waitingForResponse = true;
      lastSendTime = millis();
      expectedResponse = "LED is off";
    }

    // Process responses from clients
    if (waitingForResponse) {
      bool allClientsResponded = true;
      for (byte i = 0; i < maxClientNumber; i++) {
        if (clients[i] && clients[i].connected()) {
          if (clients[i].available()) {
            String response = clients[i].readStringUntil('\r');
            Serial.println();
            Serial.println("Client " + String(i) + " says: " + String(response));
            clients[i].flush();

            if (response == expectedResponse) {
              Serial.println("Client " + String(i) + " confirmed LED status");
            } else if (response == "LED is off") {
              LEDstatus = "low";
              Serial.println("Client " + String(i) + " confirmed LED is off");
            }
            else {
              allClientsResponded = false;
            }
          } else {
            allClientsResponded = false;
          }
        }
      }

      if (allClientsResponded) {
        if (expectedResponse == "LED is on") {
          LEDstatus = "high";
        } else if (expectedResponse == "LED is off") {
          LEDstatus = "low";
        }
        Serial.println("All clients confirmed LED status");
        waitingForResponse = false;   // Stop waiting for responses
        resendCount = 0;
        rfidReadyMessageDisplayed = false;
      } else if (millis() - lastSendTime >= resendInterval) {   // Resending LED status
        if (resendCount < maxResendAttempts) {
          Serial.println("Resending LED status to clients...");
          if (expectedResponse == "LED is on") {
            sendLedHighToClients();
          } else {
            sendLedLowToClients();
          }
          lastSendTime = millis();
          resendCount++;
        } else {
          Serial.println("Client doesn't respond");
          Serial.println();
          waitingForResponse = false; // Stop waiting after max attempts
          resendCount = 0;
          rfidReadyMessageDisplayed = false;
        }
      }
    }
  } else {
    delay(1000);
    rfidReadyMessageDisplayed = false;
    Serial.println("Error no clients connected");
  }
  unsigned long currentMillis = millis();
  unsigned long lastSetTime = 0;
  unsigned long resetInterval = 5000;
  if (LEDstatus == "on" && currentMillis - lastSetTime >= resetInterval) {
    sendLedLow = true; 
    lastSetTime = millis();
    }
}

void sendLedHighToClients() {
  for (byte i = 0; i < maxClientNumber; i++) {
    if (clients[i] && clients[i].connected()) {
      clients[i].print("LED high\r");
    }
  }
}

void sendLedLowToClients() {
  for (byte i = 0; i < maxClientNumber; i++) {
    if (clients[i] && clients[i].connected()) {
      clients[i].print("LED low\r");
    }
  }
}

// RFID
void readRFIDcard() {
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));   // Test dumps all data from RFID card

  Serial.println("Reading from RFID card...");
  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.print("  |  PICC type: ");
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);  // Model of the RFID card
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, dataKeyBlockNumber, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed for Read: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Authentication success");
  }

  status = mfrc522.MIFARE_Read(dataKeyBlockNumber, readData, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Block was read successfully");
    Serial.println("Data in block " + String(dataKeyBlockNumber) + ":");
    Serial.print(" -->");
    for (byte i = 0; i < 16; i++) {
      Serial.print(readData[i] < 10 & 0x10 ? " 0" : " ");
      Serial.print(readData[i], HEX);
    }
    Serial.println();

    byte count = 0;
    for (byte i = 0; i < 16; i++) {
      if (readData[i] == dataKey[i])
        count++;
    }
    if (count == 16) {
      sendLedHigh = true;
      Serial.println("unlocked :-)");
    } else {
      Serial.println("incorrect RFID card :-(");
      rfidReadyMessageDisplayed = false;
    }
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
