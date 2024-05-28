// TCP/IP
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

// RFID
#include <MFRC522.h>
#define SS_PIN  D8
#define RST_PIN  D4

MFRC522 mfrc522(SS_PIN, RST_PIN); // MFRC522-Instanz erstellen
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

byte myKey[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // Festlegen des Authentifizierungs-Schluessels
byte keyData[16] = {              // Festlegen des Daten-Schluessels
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0xfe, 0x3f
};
const byte keyDataBlockNumber = 2;      // Speicher Ort des Daten-Schluessel im Block
byte readData[18];
byte size = sizeof(readData);
bool rfidReadyMessageDisplayed = false; // Variable to track if the RFID ready message was displayed


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

  // RFID
  SPI.begin();
  mfrc522.PCD_Init();             // Start des RFID Sensor
  delay(4);
  for (byte i = 0; i < 6; i++)key.keyByte[i] = myKey[i]; //Key festlegen
  Serial.println();
  mfrc522.PCD_DumpVersionToSerial();  // Details vom PCI-MFRC522 RFID-Reader/Writer ausgeben
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


  if (!rfidReadyMessageDisplayed) {
    Serial.println("RFID-Reader bereit zum lesen...\n");
    rfidReadyMessageDisplayed = true; 
  }
  // Sobald eine Karte aufgelegt wird startet das Auslesen
  if (mfrc522.PICC_IsNewCardPresent()) {
    readRFIDcard(keyDataBlockNumber, readData);
    rfidReadyMessageDisplayed = false;
    /*    // 3 Sekunden pausieren um mehrfaches Lesen/Ausführen zu verhindern
        for (byte a = 3; a > 0; a--) {
          Serial.println("Bereit in: " + String(a) + "s");
          delay(1000);
        }*/
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
  Serial.println(clients[0].remoteIP()); // Client IP Adresse abfrage 
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

// RFID
void readRFIDcard(const byte keyDataBlockNumber, byte readData[]) {
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));   // Test dumpt alle Daten von RFID-Karte
  mfrc522.PICC_ReadCardSerial();

  // Zusatz Informationen
  Serial.println("Reading from RFID-Card...");
  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 16) Serial.print("0"); //Fuehrende Null anzeigen
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.print("  |  PICC type: ");
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);  // Modell der RFID-Karte
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, keyDataBlockNumber, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed for Read: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Authentication success");
  }

  status = mfrc522.MIFARE_Read(keyDataBlockNumber, readData, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Block was read successfully");
    Serial.println("Data in block " + String(keyDataBlockNumber) + ":");
    Serial.print(" -->");
    for (byte i = 0; i < 16; i++) {
      Serial.print(readData[i] < 10 & 0x10 ? " 0" : " ");
      Serial.print(readData[i], HEX);
    }
    Serial.println();

    byte count = 0;
    for (byte i = 0; i < 16; i++) {
      // Vergleicht readData mit keyData
      if (readData[i] == keyData[i])
        count++;
    }
    if (count == 16) {
      sendLedHigh = true;
      Serial.println(F("Entriegelt :-)"));
    } else {
      sendLedLow = true;
      Serial.println(F("Falsche RFID-Karte :-("));
    }
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  Serial.println();
}
