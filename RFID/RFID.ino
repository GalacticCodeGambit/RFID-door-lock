#include <SPI.h>
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


void setup() {
  Serial.begin(115200);
  SPI.begin();         
  mfrc522.PCD_Init();             // Start des RFID Sensor
  delay(4);
  for (byte i = 0; i < 6; i++)key.keyByte[i] = myKey[i]; //Key festlegen
  Serial.println();
  mfrc522.PCD_DumpVersionToSerial();  // Details vom PCI-MFRC522 RFID-Reader/Writer ausgeben
  Serial.println("RFID-Reader bereit zum lesen...");
}


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
      Serial.println(F("Entriegelt :-)"));
    } else {
      Serial.println(F("Falsche RFID-Karte :-("));
    }
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  Serial.println();
}


void loop() {
  // Sobald eine Karte aufgelegt wird startet das Auslesen
  if (mfrc522.PICC_IsNewCardPresent()) {
    readRFIDcard(keyDataBlockNumber, readData);

    // 3 Sekunden pausieren um mehrfaches Lesen/Ausführen zu verhindern
    for (byte a = 3; a > 0; a--) {
      Serial.println("Bereit in: " + String(a) + "s");
      delay(1000);
    }
    Serial.println("RFID-Reader bereit zum lesen...\n");
  }
}
