#define advanced

#ifdef advanced

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "bastian";
const char *password = "hallo123";

ESP8266WebServer server(80);

#define MAX_IP_ADDRESSES 5 // Define maximum number of IP addresses to store
IPAddress ipAddress[MAX_IP_ADDRESSES];
int numStoredIPs = 0;
bool ipStored = false;
bool unequal = true;
String str = "Hello World";

void handleRoot() {
  String message = "<html><head>";
  if (ipStored) {
    message += "<meta http-equiv='refresh' content='1'>";
  }
  message += "</head><body><h1>Enter IP Addresses:</h1>";
  message += "<form action='/store' method='get'>";
  message += "<input type='text' name='ip' placeholder='Enter IP Address'>";
  message += "<input type='submit' value='Submit'></form>";
  message += "<br>" + str + "</br>";
  message += "<h2>Stored IP Addresses:</h2><ul>";
  for (int i = 0; i < numStoredIPs; i++) {
    message += "<li>" + ipAddress[i].toString() + "</li>";
  }
  message += "</ul></body></html>";
  server.send(200, "text/html", message);
}

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
        ipStored = true;
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

void setup() {
  Serial.begin(115200);
  delay(10);

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
  server.on("/store", handleStore);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}


#else

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "bastian";
const char *password = "hallo123";

ESP8266WebServer server(80);

#define MAX_IP_ADDRESSES 5 // Define maximum number of IP addresses to store
IPAddress ipAddress[MAX_IP_ADDRESSES];
int numStoredIPs = 0;

void handleRoot() {
  String message = "<html><body><h1>Enter IP Addresses:</h1>";
  message += "<form action='/store' method='get'>";
  message += "<input type='text' name='ip' placeholder='Enter IP Address'>";
  message += "<input type='submit' value='Submit'></form>";
  message += "<h2>Stored IP Addresses:</h2><ul>";
  for (int i = 0; i < numStoredIPs; i++) {
    message += "<li>" + ipAddress[i].toString() + "</li>";
  }
  message += "</ul></body></html>";
  server.send(200, "text/html", message);
}

void handleStore() {
  String ip = server.arg("ip");
  if (ip.length() > 0 && numStoredIPs < MAX_IP_ADDRESSES) {
    Serial.println("Received IP: " + ip);
    if (ipAddress[numStoredIPs].fromString(ip)) {
      Serial.println("IP address stored successfully.");
      numStoredIPs++;
    } else {
      Serial.println("Invalid IP address format.");
    }
  } else {
    Serial.println("No IP address received or maximum limit reached.");
  }
  server.send(200, "text/plain", "IP address stored.");
}

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);
  Serial.println();

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("old");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/store", handleStore);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

#endif
