#include <EEPROM_Rotate.h>
EEPROM_Rotate EEPROMr;
#define DATA_OFFSET 10

int adr;
char data;

void setup() {
  Serial.begin(115200);
  delay(400);

  EEPROMr.size(4);
  EEPROMr.begin(4096);
  delay(400);
}

void loop() {
  delay(500);
  Serial.println("Read or Write, 'r' or 'w'");
  while (Serial.available() == 0) {}
  char c1 = Serial.read();
  Serial.flush();
  while (Serial.available()) {
    Serial.read();
  }
  delay(400);

  if (c1 == 'r') { //reading funktion
    Serial.println("Name the reading address");
    while (Serial.available() == 0) {}
    adr = Serial.parseInt(); // Convert ASCII to integer
    Serial.flush();
    while (Serial.available()) {
      Serial.read();
    }
    Serial.println("Address entered: " + String(adr));
    data = EEPROMr.read(adr);
    Serial.println("Character at chosen address: " + String(data));

  } else if (c1 == 'w') { //writing funktion
    Serial.println("Name the writing address");
    while (Serial.available() == 0) {}
    adr = Serial.parseInt(); // Convert ASCII to integer
    Serial.flush();
    while (Serial.available()) {
      Serial.read();
    }
    Serial.println("Address entered: " + String(adr));
    Serial.println("Name the character to store");
    while (Serial.available() == 0) {}
    data = Serial.read(); // Read character
    Serial.flush();
    while (Serial.available()) {
      Serial.read();
    }
    //writeToEEPROM(adr, data);
    EEPROMr.write(adr, data);
    EEPROMr.commit();
    Serial.println("Character stored: " + String(data) + " at address: " + String(adr));
  } else {
    Serial.println("Invalid input");
  }
}

//void writeToEEPROM(int adr, char data)

/*
  #include <EEPROM_Rotate.h>
  EEPROM_Rotate EEPROMr;
  #define DATA_OFFSET     10

  int adr, data, data2;
  char c1, c2, c3;


  void setup() {

  Serial.begin(9600);
  delay(400);

  EEPROMr.size(4);
  EEPROMr.begin(4096);
  delay(400);

  }

  void loop() {
  delay(500);
  Serial.println("Read or Write, 'r' or 'w'");
  while (Serial.available() == 0) {}
  c1 = Serial.read();
  while (Serial.available()) {
    Serial.read();
  }
  delay(400);
  //Serial.flush();
  delay(400);
  if (c1 == 'r') { //reading funktion
    Serial.println("name the reading adress");
    while (Serial.available() == 0) {}
    adr = Serial.read();
    while (Serial.available()) {
      Serial.read();
    }
    delay(400);
    //Serial.flush();
    delay(400);
    Serial.println("Eingabe: " + adr);
    data = EEPROMr.read(adr);
    delay(400);
    //Serial.flush();
    delay(400);
    Serial.println("zeichen in gewählter adresse ist: " + data);

  } else if (c1 == 'w') { //writing funktion
    Serial.println("name the writing adress");
    while (Serial.available() == 0) {}
    adr = Serial.read();
    while (Serial.available()) {
      Serial.read();
    }
    delay(400);
    //Serial.flush();
    delay(400);
    Serial.println("Eingabe: " + adr);
    Serial.println("name the character to store");
    while (Serial.available() == 0) {}
    c3 = Serial.read();
    while (Serial.available()) {
      Serial.read();
    }
    delay(400);
    //Serial.flush();
    delay(400);
    Serial.println("Eingabe: " + c3);
    EEPROMr.write(adr, c3);
    Serial.println("character: " + c3);
    Serial.print(" stored in adress: " + adr);
    delay(400);

  } else {
    Serial.println("ungültige eingabe");
  }
  }


  /*
   1.IP-Adresse: 0ip0.0ip1.0ip2.0ip3
   2.IP-Adresse: 1ip0.1ip1.1ip2.1ip3

   ESP_Nummer: num(1-255)



*/
