#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <ESP8266WebServer.h>

const char* ssid = "lol";
const char* passwort = "lol123456789";

/*****OLED******/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
/***********/

void setup() {
  Serial.begin(115200);
  Serial.print("\n\nVerbindungsauf zu");
  Serial.println(ssid);
  WiFi.begin(ssid, passwort);
  //warten bis Verbindung steht
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.print("\nVerbindung hergestellt.\nKijake IP:");
  Serial.println(WiFi.localIP());

  /*****OLED******/
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.display();
  delay(2000);
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("IP-Adresse: ");
  display.println( WiFi.localIP());
  display.display();
  delay(2000);
  display.clearDisplay();
  delay(1);
  /***********/
}

void loop() {


/*
  Serial.println("Error: Conection lost");
  Serial.println("Search");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Error: Conection lost");
  delay(1000);
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  delay(1000);
*/
}
