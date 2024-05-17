#include <EEPROM_Rotate.h>
EEPROM_Rotate EEPROMr;
#define DATA_OFFSET 10

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "bastian";
const char *password = "hallo123";

ESP8266WebServer server(80);

#define MAX_IP_ADDRESSES 5 // Define maximum number of IP addresses to store
IPAddress ipAddress[MAX_IP_ADDRESSES];
IPAddress ipstore[2];
IPAddress unset;

int numStoredIPs = 0;
bool ipStored = false;
bool unequal = true;
String str = "Hello World";
String ssidnew, psknew;


void handleRoot() {
  String message = "<html><head>";
  if (ipStored) {
    message += "<meta http-equiv='refresh' content='1'>";
  }

  message += "<h3>Enter IP Address to store on 1:</h3>";
  message += "<form action='/store1' method='get'>";
  message += "<input type='text' name='ip1' placeholder='" + ipstore[0].toString() + "'>";
  message += "<input type='submit' value='Submit for IP 1'></form>";
  message += "<h3>Enter IP Address to store on 2:</h3>";
  message += "<form action='/store2' method='get'>";
  message += "<input type='text' name='ip2' placeholder='" + ipstore[1].toString() + "'>";
  message += "<input type='submit' value='Submit for IP 2'></form>";
  message += "<h3>Enter ssid:</h3>";
  message += "<form action='/store3' method='get'>";
  if (ssidnew.length() != 0) {
    message += "<input type='text' name='ssid' placeholder='" + ssidnew + "'>";
  } else {
    message += "<input type='text' name='ssid' placeholder='(ssid unset)'>";
  }
  message += "<input type='submit' value='Submit for SSID'></form>";
  message += "<h3>Enter Passkey:</h3>";
  message += "<form action='/store4' method='get'>";
  if (psknew.length() != 0) {
    message += "<input type='text' name='psk' placeholder='" + psknew + "'>";
  } else {
    message += "<input type='text' name='psk' placeholder='(psk unset)'>";
  }
  message += "<input type='submit' value='Submit for Passkey'></form>";
  //message += "<br>" + str + "</br>";

  message += "<form action='/close'>";
  message += "<input type='submit' value='Store&Reeboot'></form>";

  server.send(200, "text/html", message);
}

/*
  void handleStore() {
  String ip = server.arg("ip");
  if (ip.length() > 0 && numStoredIPs < MAX_IP_ADDRESSES) {
    Serial.println("Received IP: " + ip);
    if (ipAddress[numStoredIPs].fromString(ip)) {
      for(int i = 0; i < numStoredIPs; i++){
        if(ipAddress[i] == ipAddress[numStoredIPs]){
          unequal = false;
          Serial.println("IP-Address is already stored!");
          str = "IP-Address is already stored!";
        }
      }
      if(unequal){
        Serial.println("IP address stored successfully.");
        str = "IP address stored successfully.";
        numStoredIPs++;
      } else {
        unequal = true;
      }
    } else {
      Serial.println("Invalid IP address format.");
      str = "Invalid IP address format.";
    }
  } else {
    Serial.println("No IP address received or maximum limit reached.");
    str = "No IP address received or maximum limit reached.";
  }
  ipStored = false;
  server.sendHeader("Location", "/", true);   // Redirect to root page
  server.send(303);  // Send response with redirect
  }
*/

// beginning
void handleStore1() { //storing IPAddress 1
  String ip1 = server.arg("ip1");
  if (ip1.length() > 0 && numStoredIPs < MAX_IP_ADDRESSES) {
    Serial.println("Received IP for IP 1: " + ip1);
    if (ipstore[0].fromString(ip1)) {
      Serial.println("IP address 1 stored successfully.");
    } else {
      Serial.println("Invalid IP address format for IP 1.");
    }
  } else {
    Serial.println("No IP address received for IP 1 or maximum limit reached.");
  }
  ipStored = false;

  server.sendHeader("Location", "/", true);   // Redirect to root page
  server.send(303);  // Send response with redirect
}

void handleStore2() { //storing IPAddress 2
  String ip2 = server.arg("ip2");
  if (ip2.length() > 0 && numStoredIPs < MAX_IP_ADDRESSES) {
    Serial.println("Received IP for IP 2: " + ip2);
    if (ipstore[1].fromString(ip2)) {
      Serial.println("IP address 2 stored successfully.");
    } else {
      Serial.println("Invalid IP address format for IP 2.");
    }
  } else {
    Serial.println("No IP address received for IP 2 or maximum limit reached.");
  }
  ipStored = false;

  server.sendHeader("Location", "/", true);   // Redirect to root page
  server.send(303);  // Send response with redirect
}

void handleStore3() { //storing SSID
  ssidnew = server.arg("ssid");

  Serial.println("Received ssid: " + ssidnew);

  ipStored = false;

  server.sendHeader("Location", "/", true);   // Redirect to root page
  server.send(303);  // Send response with redirect
}

void handleStore4() { //storing Passkey
  psknew = server.arg("psk");

  Serial.println("Received psk: " + psknew);

  ipStored = false;

  server.sendHeader("Location", "/", true);   // Redirect to root page
  server.send(303);  // Send response with redirect
}
// ending
// EEPROM handle
void handleclose() {

  String message = "<html><head>";

  message += "<h3>Enter IP Address to store on 1:</h3>";
  message += "<form action='/store1' method='get'>";
  message += "<input type='text' name='ip1' placeholder='" + ipstore[0].toString() + "'>";
  message += "<input type='submit' value='Submit for IP 1'></form>";
  message += "<h3>Enter IP Address to store on 2:</h3>";
  message += "<form action='/store2' method='get'>";
  message += "<input type='text' name='ip2' placeholder='" + ipstore[1].toString() + "'>";
  message += "<input type='submit' value='Submit for IP 2'></form>";
  message += "<h3>Enter ssid:</h3>";
  message += "<form action='/store3' method='get'>";
  if (ssidnew.length() != 0) {
    message += "<input type='text' name='ssid' placeholder='" + ssidnew + "'>";
  } else {
    message += "<input type='text' name='ssid' placeholder='(ssid unset)'>";
  }
  message += "<input type='submit' value='Submit for SSID'></form>";
  message += "<h3>Enter Passkey:</h3>";
  message += "<form action='/store4' method='get'>";
  if (psknew.length() != 0) {
    message += "<input type='text' name='psk' placeholder='" + psknew + "'>";
  } else {
    message += "<input type='text' name='ssid' placeholder='(psk unset)'>";
  }
  message += "<input type='submit' value='Submit for Passkey'></form>";
  message += "<br>Good Night World</br>";

  server.send(200, "text/html", message);     //end of playing

  //EEPROMr.size(4);
  //EEPROMr.begin(4096);
  int i, j;
  for (j = 0; j < 2; j++) {
    if (ipstore[j] != unset) {
      for (i = 0; i < 4; i++) {
        EEPROMr.write(DATA_OFFSET + (4 * j) + i, ipstore[j][i]);
      }
      delay(100);
    }
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

  message = "<h1>Storing successful</h1>";
  server.send(200, "text/html", message);
}

void readall() {
  EEPROMr.size(4);
  EEPROMr.begin(4096);

  int i, j;
  for (j = 0; j < 2; j++) {
    for (i = 0; i < 4; i++) {
      ipstore[j][i] = EEPROMr.read(DATA_OFFSET + (4 * j) + i);
    }
  }
  i = 0;
  j = 0;


  /*while ((EEPROMr.read(100 + i) != ';') && (i < 100)) {
    ssidnew[i] = EEPROMr.read(100 + i);
    if ((i == 99) && (EEPROMr.read(199) != ';')) {
      ssidnew = "";
    }
    i++;
    }*/
  while (i < 100) {
    char c = EEPROMr.read(100 + i);
    if (c == ';') {
      break; 
    }
    ssidnew[i] = c;
    i++;
  }
  if (i == 100) {
    ssidnew = "";
  }


  while ((EEPROMr.read(200 + j) != ';') && (j < 100)) {
    psknew[j] = EEPROMr.read(200 + j);
    if ((j == 99) && (EEPROMr.read(299) != ';')) {
      psknew = "";
    }
    j++;
  }

}

void setup() {
  Serial.begin(115200);
  delay(200);

  readall();

  WiFi.begin(ssid, password);
  Serial.println();

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("advanced");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  //server.on("/store", handleStore);
  server.on("/store1", handleStore1);
  server.on("/store2", handleStore2);
  server.on("/store3", handleStore3);
  server.on("/store4", handleStore4);
  server.on("/close", handleclose);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
