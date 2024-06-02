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

#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define DATA_OFFSET 10

#define APSSID "ESPap"
#define APPSK "thereisnospoon"

const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

IPAddress gateway;
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

IPAddress ipstore, unset;
bool ipStored = false;
String str = "Hello World";
String ssidnew, psknew;
char strvar[101];
bool srun = true;
bool showPassword = false;
bool wifiBool = true;
bool autoIPassignBool = false;

int clientnumber;
const int maxClientNumber = 5;
IPAddress clientIP[255];
WiFiClient clients[maxClientNumber];
bool lock = true;

String lockStatus;

void handleRoot() { //Root handler for Wifi and Access Point connection || Wifi + AP
  String message = "<html><head>";
  if (wifiBool) { //wifi mode
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

    message += "<body><h1>Configuration Page</h1>";
    message += "<body><h3>Enter static IP Address for ESP1:</h3>";
    message += "<form action='/sub' method='get'>";
    message += "<input type='text' name='ip' value='" + ipstore.toString() + "'>";

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

  server.send(200, "text/html", message);
}

void handleShowPassword() { //switch bool to show/unshow Password || AP
  showPassword = !showPassword;
  server.sendHeader("Location", "/", true);
  server.send(303);
}

void handlesub() {  //submites all written values into variables || AP
  str = "Storing successful";
  String ip = server.arg("ip");
  if (ip.length() > 0) {
    Serial.println("Received IP: " + ip);
    if (ipstore.fromString(ip)) {
      Serial.println("IP Address stored successfully.");
    } else {
      Serial.println("Invalid IP Address format for IP.");
    }

  } else {
    Serial.println("No IP address received for 'IP'.");
  }

  if (server.arg("ssid").length() > 0) {
    ssidnew = server.arg("ssid");
  }
  if (server.arg("psk").length() > 0) {
    psknew = server.arg("psk");
  }

  ipStored = false;

  server.sendHeader("Location", "/", true);
  server.send(303);
}

void handleclose() {  //stops the Access Point mode and switch to Wifi mode || stores variables in EEPROM || AP
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
  message += "<input type='text' name='ip' value='" + ipstore.toString() + "'>";

  message += "<h3>Enter ssid:</h3>";
  message += "<input type='text' name='ssid' value='" + ssidnew + "' placeholder='(ssid unset)'>";

  message += "<h3>Enter Passkey:</h3>";
  message += "<input type='password' id='psk' name='psk' value='" + psknew + "' placeholder='(psk unset)'>";
  message += "<button type='button' onclick='togglePassword()'>Toggle Password</button>";

  message += "<br></br><br>Good Night World</br>";
  message += "</body>";
  message += "</html>";

  server.send(200, "text/html", message);

  int i;
  if (ipstore != unset) {
    for (i = 0; i < 4; i++) {
      EEPROMr.write(DATA_OFFSET + i, ipstore[i]);
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

void readall() {  //reads values for Variables from EEPROM
  int i, j;
  for (i = 0; i < 4; i++) {
    ipstore[i] = EEPROMr.read(DATA_OFFSET + i);
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

void handleGetLockStatus() {  //let the Website Autorenew the Lockstate || Wifi
  lockStatus = lock ? "locked" : "unlocked";
  server.send(200, "text/plain", lockStatus);
}

void handleIPrenew () { //loads new list of connected Clients || Wifi
  clientnumber = 0;
  for (int i = 0; i < maxClientNumber; i++) {
    if (clients[i] && clients[i].connected()) {
      clientIP[clientnumber] = clients[i].remoteIP();
      clientnumber++;
    }
  }
  server.sendHeader("Location", "/", true);   // Redirect to root page
  server.send(303);  // Send response with redirect
}

void handleAP() { //goes into Access Point mode || Wifi to AP
  APmode();
  reconnect1(1);
}

void handleautoIPassign() { //let the DHCP assign the IP-Address || AP to Wifi
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

  ipstore = WiFi.localIP();

  gateway = ipstore;
  gateway[3] = 1;

  for (int i = 0; i < 4; i++) {
    EEPROMr.write(DATA_OFFSET + i, ipstore[i]);
  }
  delay(200);
  EEPROMr.commit();
}

void APmode() { //Access Point mode
  wifiBool = false;
  WiFi.softAP(ssid);
  Serial.println();

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("AP Online\n");
  display.println(myIP);
  display.display();

  Serial.println("HTTP server started");

  bool angemeldet = true;
  while (srun) {
    server.handleClient();
    if (WiFi.softAPgetStationNum() == 0 && angemeldet) {
      angemeldet = false;
      startTime = millis(); //setzt Startzeit
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

    //new IP assurence
    gateway = ipstore;
    gateway[3] = 1;
    if (ipstore != unset && ipstore != gateway) WiFi.config(ipstore, gateway, subnet);
  }
  autoIPassignBool = false;
}

void operatingproof() { //checks, if static IP is working with the Network
  if (Ping.ping(gateway)) {
    Serial.println("Ping successful, static IP is operable");
  } else {
    Serial.println("Ping failed, static IP is not operable");
    str = "Ping failed, static IP is not operable";
    APmode();
  }
}

void reconnect1(int s) {  //if not reconnect, then go back in Access Point mode
  readall();
  int i = 0;
  int minToAccessPointMode = 3;
  display.clearDisplay();
  display.setCursor(0, 0);  //Set the cursor after clearing the display
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
  display.display(); // Update the display with the new content

  display.setTextColor(WHITE);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    if (i == (minToAccessPointMode * 60)) { // After 'minToAccessPointMode' minutes the ESP is set to Access Point mode
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Connection failure");
      display.display();
      delay(1000);
      APmode();
    }
    i++;
  }
  operatingproof();
}

void setup() {
  EEPROMr.size(4);
  EEPROMr.begin(4096);
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(200);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  readall();

  lockStatus = lock ? "locked" : "unlocked";

  delay(500);
  gateway = ipstore;
  gateway[3] = 1;
  if (ipstore != unset && ipstore != gateway) WiFi.config(ipstore, gateway, subnet);
  WiFi.begin(ssidnew, psknew);

  //wifi + AP mode
  server.on("/", handleRoot);

  //wifi mode
  server.on("/iprenew", handleIPrenew);
  server.on("/getLockStatus", handleGetLockStatus);
  server.on("/ap", handleAP);

  //AP mode
  server.on("/showPassword", handleShowPassword);
  server.on("/sub", handlesub);
  server.on("/close", handleclose);
  server.on("/autoIPassign", handleautoIPassign);
  server.begin();

  reconnect1(0);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  /// TCP/IP
  server.handleClient();

  if (WiFi.status() != WL_CONNECTED) {  //reeconeting falls verbindungsabbruch
    reconnect1(1);
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connected");
  display.println(WiFi.localIP());
  display.setCursor(0, 40);
  display.println(lock ? "locked" : "unlocked");
  display.display();
  delay(200);

  // Simulate lock status change for demonstration
  static unsigned long lastChange = 0;
  if (millis() - lastChange > 5000) { // Change status every 5 seconds
    lock = !lock;
    lastChange = millis();
  }
}
