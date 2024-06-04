/***************/
#define DEBUG 1                            // To enable serial output, set debug equal to 1, to disable set it to not equal 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugHEX(x) Serial.print(x, HEX)
#else
#define debug(x)
#define debugln(x)
#endif
/***************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <EEPROM_Rotate.h>
EEPROM_Rotate EEPROMr;
#define DATA_OFFSET 10

unsigned long startTime;

#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>

#define OLED_RESET 0                       // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define DATA_OFFSET 10

#define APSSID "ESP1ap"
#define APPSK "thereisnospoon"

const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer httpServer(80);

IPAddress gateway;
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

IPAddress ServerIP, unset;
bool ipStored = false;
String str = "Hello World";
String ssidnew, psknew;
char strvar[101];
bool srun = true;
bool showPassword = false;
bool wifiBool = true;
bool autoIPassignBool = false;

// TCP/IP
WiFiServer tcpServer(40);                  // TCP/IP Server Port
int clientnumber;
const byte maxClientNumber = 5;
IPAddress clientIP[maxClientNumber];
WiFiClient clients[maxClientNumber];
bool lockStatusBool = true;
String lockStatus;

String LEDstatus = "low";                  // Current status of LED

unsigned long currentMillis = 0;
unsigned long lastSetTime = 0;
const unsigned long resetInterval = 5000;

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


void handleRoot() {                        // Root handler for Wifi and Access Point connection || Wifi + AP
  String message = "<html><head>";
  if (wifiBool) {                          // WiFi mode
    message += "<meta charset='UTF-8'>";
    message += "<title>Doorlock.web</title>";
    message += "<script>";
    message += "function updateLockStatus() {";
    message += "  var xhr = new XMLHttpRequest();";
    message += "  xhr.open('GET', '/getLockStatus', true);";
    message += "  xhr.onreadystatechange = function() {";
    message += "    if (xhr.readyState == 4 && xhr.status == 200) {";
    message += "      document.getElementById('lockStatus').innerText = xhr.responseText;";
    message += "    }";
    message += "  };";
    message += "  xhr.send();";
    message += "}";
    message += "setInterval(updateLockStatus, 1000);";  // Update every second
    message += "</script>";
    message += "</head><body>";
    message += "<h1>Doorlock.web</h1>";
    message += "<p>Lock Status: <span id='lockStatus'>" + lockStatus + "</span></p>";
    message += "<ul>";
    for (int i = 0; i < clientnumber; i++) {
      message += "<li>" + clientIP[i].toString() + "</li>";
    }
    message += "</ul><form action='/iprenew'>";
    message += "<input type='submit' value='Renew IP Address list'></form>";
    message += "<form action='/ap'><input type='submit' value='APmode'></form>";
  } else {  //AP mode
    message += "<title>Configuration Page</title>";
    message += "<script>";
    message += "function togglePassword() {";
    message += "  var x = document.getElementById('psk');";
    message += "  if (x.type === 'password') {";
    message += "    x.type = 'text';";
    message += "  } else {";
    message += "    x.type = 'password';";
    message += "  }";
    message += "}";
    message += "</script></head>";

    message += "<body><h1>Configuration Page ESP1</h1>";
    message += "<body><h3>Enter static IP Address for ESP1:</h3>";
    message += "<form action='/sub' method='get'>";
    message += "<input type='text' name='ip' value='" + ServerIP.toString() + "'>";

    message += "<h3>Enter ssid:</h3>";
    message += "<input type='text' name='ssid' value='" + ssidnew + "' placeholder='(ssid unset)'>";

    message += "<h3>Enter Passkey:</h3>";
    message += "<input type='password' id='psk' name='psk' value='" + psknew + "' placeholder='(psk unset)'>";
    message += "<button type='button' onclick='togglePassword()'>Toggle Password</button>";

    message += "<br></br><br>" + str + "</br><br></br>";
    message += "<input type='submit' value='Submit all'></form>";

    message += "<form action='/close'>";
    message += "<input type='submit' value='Store in EEPROM & Reboot'></form>";

    message += "<form action='/autoIPassign'>";
    message += "<input type='submit' value='automatic IP address assignment'></form>";
  }
  message += "</body></html>";

  httpServer.send(200, "text/html", message);
}

void handleShowPassword() {                // Switch bool to show/unshow Password || AP
  showPassword = !showPassword;
  httpServer.sendHeader("Location", "/", true);
  httpServer.send(303);
}

void handlesub() {                         // Submites all written values into variables || AP
  str = "Storing successful";
  String ip = httpServer.arg("ip");
  if (ip.length() > 0) {
    debugln("Received IP: " + ip);
    if (ServerIP.fromString(ip)) {
      debugln("IP Address stored successfully.");
    } else {
      debugln("Invalid IP Address format for IP.");
    }

  } else {
    debugln("No IP address received for 'IP'.");
  }

  if (httpServer.arg("ssid").length() > 0) {
    ssidnew = httpServer.arg("ssid");
  }
  if (httpServer.arg("psk").length() > 0) {
    psknew = httpServer.arg("psk");
  }

  ipStored = false;

  httpServer.sendHeader("Location", "/", true);
  httpServer.send(303);
}

void handleclose() {                       // Stops the Access Point mode and switch to Wifi mode || stores variables in EEPROM || AP
  String message = "<html><head>";
  message += "<meta http-equiv='refresh' content='10'>";
  message += "<script>";
  message += "function togglePassword() {";
  message += "  var x = document.getElementById('psk');";
  message += "  if (x.type === 'password') {";
  message += "    x.type = 'text';";
  message += "  } else {";
  message += "    x.type = 'password';";
  message += "  }";
  message += "}";
  message += "</script></head>";

  message += "<body><h3>Enter IP Address to store:</h3>";
  message += "<form action='/sub' method='get'>";
  message += "<input type='text' name='ip' value='" + ServerIP.toString() + "'>";

  message += "<h3>Enter ssid:</h3>";
  message += "<input type='text' name='ssid' value='" + ssidnew + "' placeholder='(ssid unset)'>";

  message += "<h3>Enter Passkey:</h3>";
  message += "<input type='password' id='psk' name='psk' value='" + psknew + "' placeholder='(psk unset)'>";
  message += "<button type='button' onclick='togglePassword()'>Toggle Password</button>";

  message += "<br></br><br>Good Night World</br>";
  message += "</body>";
  message += "</html>";

  httpServer.send(200, "text/html", message);

  int i;
  if (ServerIP != unset) {
    for (i = 0; i < 4; i++) {
      EEPROMr.write(DATA_OFFSET + i, ServerIP[i]);
    }
    delay(100);
  }

  if (ssidnew.length() != 0) {
    i = 0;
    while (i < ssidnew.length()) {
      EEPROMr.write(100 + i, ssidnew[i]);
      i++;
      delay(100);
    }
    EEPROMr.write(100 + i, ';');
  }

  if (psknew.length() != 0) {
    i = 0;
    while (i < psknew.length()) {
      EEPROMr.write(200 + i, psknew[i]);
      i++;
      delay(100);
    }
    EEPROMr.write(200 + i, ';');
  }
  delay(100);

  EEPROMr.commit();
  delay(1000);

  srun = false;
}

void readAll() {                           // Reads values for Variables from EEPROM
  int i, j;
  for (i = 0; i < 4; i++) {
    ServerIP[i] = EEPROMr.read(DATA_OFFSET + i);
  }
  i = 0;
  j = 0;

  while (i < 100) {
    char c = EEPROMr.read(100 + i);
    if (c == ';') {
      break;
    }
    strvar[i] = c;
    i++;
    delay(100);
  }
  strvar[i] = '\0';
  if (i != 100) {
    ssidnew = String(strvar);
  }

  memset(strvar, '\0', sizeof(strvar));

  while (j < 100) {
    char c = EEPROMr.read(200 + j);
    if (c == ';') {
      break;
    }
    strvar[j] = c;
    j++;
    delay(100);
  }
  strvar[j] = '\0';
  if (j != 100) {
    psknew = String(strvar);
  }
}

void handleGetLockStatus() {               // Let the Website Autorenew the Lockstate || Wifi
  lockStatus = lockStatusBool ? "locked" : "unlocked";
  httpServer.send(200, "text/plain", lockStatus);
}

void handleIPrenew () {                          // Loads new list of connected Clients || Wifi
  clientnumber = 0;
  for (int i = 0; i < maxClientNumber; i++) {
    if (clients[i] && clients[i].connected()) {
      clientIP[clientnumber] = clients[i].remoteIP();
      clientnumber++;
    }
  }
  httpServer.sendHeader("Location", "/", true);  // Redirect to root page
  httpServer.send(303);                          // Send response with redirect
}

void handleAP() {                           // Goes into Access Point mode || Wifi to AP
  APmode();
  reconnect1(1);
}

void handleAutoIPassign() {                 // Let the DHCP assign the IP-Address || AP to Wifi
  autoIPassignBool = true;

  WiFi.softAPdisconnect(true);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Assigning IP automaticly");
  display.println("Rebooting");
  display.display();

  WiFi.disconnect(true);
  delay(200);
  WiFi.begin(ssidnew, psknew);

  reconnect1(0);

  ServerIP = WiFi.localIP();

  gateway = ServerIP;
  gateway[3] = 1;

  byte i;
  for  (i = 0; i < 4; i++) {
    EEPROMr.write(DATA_OFFSET + i, ServerIP[i]);
  }
  if (ssidnew.length() != 0) {
    i = 0;
    while (i < ssidnew.length()) {
      EEPROMr.write(100 + i, ssidnew[i]);
      i++;
      delay(100);
    }
    EEPROMr.write(100 + i, ';');
  }
  if (psknew.length() != 0) {
    i = 0;
    while (i < psknew.length()) {
      EEPROMr.write(200 + i, psknew[i]);
      i++;
      delay(100);
    }
    EEPROMr.write(200 + i, ';');
  }
  delay(200);
  EEPROMr.commit();
}

void APmode() {                             // Access Point mode
  wifiBool = false;
  WiFi.softAP(ssid);
  debugln();

  IPAddress myIP = WiFi.softAPIP();
  debug("AP IP address: ");
  debugln(myIP);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("AP Online\n");
  display.println(myIP);
  display.display();

  debugln("HTTP server started");

  bool angemeldet = true;
  while (srun) {
    httpServer.handleClient();
    if (WiFi.softAPgetStationNum() == 0 && angemeldet) {
      angemeldet = false;
      startTime = millis();                 // Sets start time
    } else if (WiFi.softAPgetStationNum() > 0 && !angemeldet) {
      angemeldet = true;
    } else if ((millis() - startTime) > (2 * 60 * 1000) && !angemeldet) {
      srun = false;
    }
  }
  srun = true;
  wifiBool = true;
  if (!autoIPassignBool) {
    WiFi.softAPdisconnect(true);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Operation complete");
    display.println("Rebooting");
    display.display();
    delay(500);

    // New IP assurance
    gateway = ServerIP;
    gateway[3] = 1;
    if (ServerIP != unset && ServerIP != gateway) WiFi.config(ServerIP, gateway, subnet);
  }
  autoIPassignBool = false;
}

void operatingProof() {                     // Check, if static IP is working with the Network
  debugln();
  if (Ping.ping(gateway)) {
    debugln("Ping successful, static IP is operable");
  } else {
    debugln("Ping failed, static IP is not operable");
    str = "Ping failed, static IP is not operable";
    APmode();
  }
}

void reconnect1(int s) {                    // If not reconnect, then go back in Access Point mode
  readAll();
  int i = 0;
  int minToAccessPointMode = 3;
  display.clearDisplay();
  display.setCursor(0, 0);                  // Set the cursor after clearing the display
  switch (s) {
    case 0:
      display.println("Connecting");
      break;
    case 1:
      display.setTextColor(BLACK, WHITE);
      display.println("reconnecting");
      break;
    default:
      display.println("u r a Failure");
      break;
  }
  display.display();                        // Update the display with the new content

  display.setTextColor(WHITE);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    debug(".");
    if (i == (minToAccessPointMode * 60)) {  // After 'minToAccessPointMode' minutes the ESP is set to Access Point mode
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Connection failure");
      display.display();
      delay(1000);
      APmode();
    }
    i++;
  }
  operatingProof();
}

void tcpLoop() {
  if (tcpServer.hasClient()) {              // Accept new clients
    for (byte i = 0; i < maxClientNumber; i++) {
      // Find a free slot
      if (!clients[i] || !clients[i].connected()) {
        if (clients[i]) {
          clients[i].stop();
        }
        clients[i] = tcpServer.available();
        debug("New client connected, ID: ");
        debugln(i);
        break;
      }
    }
  }


  byte connectedClients = 0;                // Counts the connected clients
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
      debugln("\nRFID-Reader ready to read...\n\n");
      rfidReadyMessageDisplayed = true;
    }

    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {  // Starts reading as soon as a card is placed
      readRFIDcard();
    }

    if (waitingForResponse) {               // Process responses from clients
      bool allClientsResponded = true;
      for (byte i = 0; i < maxClientNumber; i++) {
        if (clients[i] && clients[i].connected()) {
          if (clients[i].available()) {
            String response = clients[i].readStringUntil('\r');
            debugln("Client " + String(i) + " says: " + String(response));
            clients[i].flush();
            if (response == expectedResponse) {
              debugln("Client " + String(i) + " confirmed LED status");
            } else if (response == "LED is off") {
              LEDstatus = "low";
              debugln("Client " + String(i) + " confirmed LED is off");
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
        debugln("All clients confirmed LED status");
        waitingForResponse = false;         // Stop waiting for responses
        resendCount = 0;
        rfidReadyMessageDisplayed = false;
      } else if (millis() - lastSendTime >= resendInterval) {   // Resending LED status
        if (resendCount < maxResendAttempts) {
          if (expectedResponse == "LED is on") {
            debugln("Resending LED status high to clients...");
            sendLedHighToClients();
          } else if (expectedResponse == "LED is off") {
            debugln("Resending LED status low to clients...");
            //debugln("Send low to Clients");
            sendLedLowToClients();
          }
          lastSendTime = millis();
          resendCount++;
        } else {
          debugln("Client doesn't respond");
          waitingForResponse = false;       // Stop waiting after max attempts
          resendCount = 0;
          rfidReadyMessageDisplayed = false;
        }
      }
    }
    currentMillis = millis();
    if (LEDstatus == "high" && currentMillis - lastSetTime >= resetInterval) {  // LED auf off reseten
      debugln("Send low to Clients");
      sendLedLowToClients();
      lastSetTime = millis();
    }
  } else {
    delay(1000);
    rfidReadyMessageDisplayed = false;
    debugln("Error no clients connected");
  }


}


void sendLedHighToClients() {               // Send "LED high" to all connected clients
  for (byte i = 0; i < maxClientNumber; i++) {
    if (clients[i] && clients[i].connected()) {
      clients[i].print("LED high\r");
    }
  }
  waitingForResponse = true;
  lastSendTime = millis();
  expectedResponse = "LED is on";
  lastSetTime = millis();
}

void sendLedLowToClients() {                // Send "LED low" to all connected clients
  for (byte i = 0; i < maxClientNumber; i++) {
    if (clients[i] && clients[i].connected()) {
      clients[i].print("LED low\r");
    }
  }
  waitingForResponse = true;
  lastSendTime = millis();
  expectedResponse = "LED is off";
}

void readRFIDcard() {
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));  // Test dumps all data from RFID card

  debugln("Reading from RFID card...");
  debug("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    debugHEX(mfrc522.uid.uidByte[i]);
    debug(" ");
  }
  debug("  |  PICC type: ");
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);  // Model of the RFID card
  debugln(mfrc522.PICC_GetTypeName(piccType));

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, dataKeyBlockNumber, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    debug("Authentication failed for Read: ");
    debugln(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    debugln("Authentication success");
  }

  status = mfrc522.MIFARE_Read(dataKeyBlockNumber, readData, &len);
  if (status != MFRC522::STATUS_OK) {
    debug("Reading failed: ");
    debugln(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    debugln("Block was read successfully");
    debugln("Data in block " + String(dataKeyBlockNumber) + ":");
    debug(" -->");
    for (byte i = 0; i < 16; i++) {
      debug(readData[i] < 10 & 0x10 ? " 0" : " ");
      debugHEX(readData[i]);
    }
    debugln();

    byte equalCount = 0;
    for (byte i = 0; i < 16; i++) {
      if (readData[i] == dataKey[i])
        equalCount++;
    }
    if (equalCount == 16) {
      debugln("unlocked :-)");
      debugln();
      debugln("Send high to Clients");
      sendLedHighToClients();
    } else {
      debugln("incorrect RFID card :-(");
      rfidReadyMessageDisplayed = false;
    }
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void setup() {
  EEPROMr.size(4);
  EEPROMr.begin(4096);
  Serial.begin(115200);
  debugln();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(200);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  readAll();

  lockStatus = lockStatusBool ? "locked" : "unlocked";

  delay(500);
  gateway = ServerIP;
  gateway[3] = 1;
  if (ServerIP != unset && ServerIP != gateway) WiFi.config(ServerIP, gateway, subnet);
  WiFi.begin(ssidnew, psknew);

  // WiFi + AP mode
  httpServer.on("/", handleRoot);

  // WiFi mode
  httpServer.on("/iprenew", handleIPrenew);
  httpServer.on("/getLockStatus", handleGetLockStatus);
  httpServer.on("/ap", handleAP);

  // AP mode
  httpServer.on("/showPassword", handleShowPassword);
  httpServer.on("/sub", handlesub);
  httpServer.on("/close", handleclose);
  httpServer.on("/autoIPassign", handleAutoIPassign);
  httpServer.begin();

  reconnect1(0);

  tcpServer.begin();                        // Starts the TCP server

  debugln();
  debugln("WiFi connected and TCP-Server started");
  debugln("IP address: ");
  debugln(WiFi.localIP());

  // RFID
  SPI.begin();
  mfrc522.PCD_Init();                       // Starting the RFID sensor
  delay(4);
  for (byte i = 0; i < 6; i++)key.keyByte[i] = authenticationKey[i];  // Set key
}

void loop() {
  httpServer.handleClient();

  if (WiFi.status() != WL_CONNECTED) {      // Reconnect if connection is lost
    reconnect1(1);
  }
  
  tcpLoop();
  
  lockStatusBool = (LEDstatus == "low") ? true : false;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connected");
  display.println(WiFi.localIP());
  display.setCursor(0, 40);
  display.println(lockStatusBool ? "locked" : "unlocked");
  display.display();
  delay(200);
}
