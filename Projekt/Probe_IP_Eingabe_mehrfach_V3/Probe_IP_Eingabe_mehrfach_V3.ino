#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "bastian";
const char *password = "hallo123";

ESP8266WebServer server(80);

#define MAX_IP_ADDRESSES 5 // Define maximum number of IP addresses to store
IPAddress ipAddress[MAX_IP_ADDRESSES];
IPAddress ipstore[2];

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
  message += "<h2>Enter IP Address to store on 1:</h2>";
  message += "<form action='/store1' method='get'>";
  message += "<input type='text' name='ip1' placeholder='Enter IP Address'>";
  message += "<input type='submit' value='Submit for IP 1'></form>";
  message += "<h2>Enter IP Address to store on 2:</h2>";
  message += "<form action='/store2' method='get'>";
  message += "<input type='text' name='ip2' placeholder='Enter IP Address'>";
  message += "<input type='submit' value='Submit for IP 2'></form>";
  message += "<br>" + str + "</br>";
  message += "<h2>Stored IP Addresses:</h2><ul>";
  for (int i = 0; i < numStoredIPs; i++) {
    message += "<li>" + ipAddress[i].toString() + "</li>";
  }
  message += "</ul><br></br>";
  message += "<h2>Stored IP Addresses1</h2>";
  message += ipstore[0].toString(); 
  message += "<br></br><h2>Stored IP Addresses2</h2>";
  message += ipstore[1].toString(); 
  message += "</body></html>";
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


// beginning
void handleStore1() {
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

void handleStore2() {
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
// ending

void setup() {
  Serial.begin(115200);
  delay(200);

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
  server.on("/store1", handleStore1);
  server.on("/store2", handleStore2);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
