#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <EEPROM_Rotate.h>
EEPROM_Rotate EEPROMr;
#define DATA_OFFSET 10

#include <ESP8266WiFi.h>
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
const char *host = "esp2";

ESP8266WebServer server(80);

IPAddress ipstore, unset;
bool ipStored = false;
String str = "Hello World";
String ssidnew, psknew;
char strvar[101];
bool serverRun = true;
bool showPassword = false;

#define LED_Pin D4

void handleRoot() { //Root handler for Access Point
  String message = "<html><head><title>Configuration Page</title>";
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
  message += "<h3>Enter IP Address of ESP1:</h3>";
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
  message += "<input type='submit' value='Store in EEPROM & Reboot'></form></body></html>";

  server.send(200, "text/html", message);
}

void handleShowPassword() { //switch bool to show/unshow Password
  showPassword = !showPassword;
  server.sendHeader("Location", "/", true);
  server.send(303);
}

void handlesub() {  //submites all written values into variables
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
  str = "Storing successful";

  server.sendHeader("Location", "/", true);
  server.send(303);
}

void handleclose() {  //stops the Access Point mode and stores variables in EEPROM
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

  server.stop();
  serverRun = false;
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

void APmode() { //Access Point mode
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

  server.on("/", handleRoot);
  server.on("/showPassword", handleShowPassword);
  server.on("/sub", handlesub);
  server.on("/close", handleclose);
  server.begin();
  Serial.println("HTTP server started");

  while (serverRun) {
    server.handleClient();
  }

  WiFi.softAPdisconnect(true);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Operation complete");
  display.println("Rebooting");
  display.display();


}

void reconnect1(int state) {  //if not reconnect, then go back in Access Point mode
  int i = 0;
  int minToAccessPointMode = 3;      
  display.clearDisplay();
  display.setCursor(0, 0); // Set the cursor after clearing the display
  switch (state) {
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
      //ESP.restart();
      APmode();
    }
    i++;
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connected");
  display.println(WiFi.localIP());
  display.display();

}

void setup() {
  EEPROMr.size(4);
  EEPROMr.begin(4096);
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(200);

  pinMode(LED_Pin, OUTPUT);
  digitalWrite(LED_Pin, 0);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  readall();

  delay(500);
  WiFi.begin(ssidnew, psknew);

  reconnect1(0);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
 /// TCP/IP
  if (WiFi.status() != WL_CONNECTED) {  //reeconeting falls verbindungsabbruch
    reconnect1(1);
  }
  delay(200);
}
