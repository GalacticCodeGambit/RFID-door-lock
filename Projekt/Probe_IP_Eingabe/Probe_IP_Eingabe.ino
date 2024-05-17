#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "bastian";
const char *password = "hallo123";

ESP8266WebServer server(80);

IPAddress ipAddress;

void handleRoot() {
  String message = "<html><body><h1>Enter IP Address:</h1>";
  message += "<form action='/store' method='get'>";
  message += "<input type='text' name='ip' placeholder='Enter IP Address'>";
  message += "<input type='submit' value='Submit'></form>";
  message += "</body></html>";
  server.send(200, "text/html", message);
}

void handleStore() {
  String ip = server.arg("ip");
  if (ip.length() > 0) {
    Serial.println("Received IP: " + ip);
    if (ipAddress.fromString(ip)) {
      Serial.println("IP address stored successfully.");
      // You can use ipAddress variable for further operations
    } else {
      Serial.println("Invalid IP address format.");
    }
  } else {
    Serial.println("No IP address received.");
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
