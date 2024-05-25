#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  15     // Pin: D8  15
#define RST_PIN  2     // Pin: D4  2

long chipUID;

MFRC522 mfrc522(SS_PIN, RST_PIN); // MFRC522-Instanz erstellen
MFRC522::MIFARE_Key key;

byte myKey[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // Festlegen des Authentifizierungs-Schluessels
byte keyData[16] = {      // Festlegen des Daten-Schluessels
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0xfe, 0x3f
};

byte blockNum = 2;      // Nummer des auszulesenden Datenblockes für den Daten-Schluessel
byte readData[18];
byte size = sizeof(readData);

MFRC522::StatusCode status;


void setup() {
  Serial.begin(115200);
  SPI.begin();         // SPI-Bus initialisieren
  mfrc522.PCD_Init();  // Startet RFID Sensor
  delay(4);
  for (byte i = 0; i < 6; i++)key.keyByte[i] = myKey[i]; //Key festlegen
  Serial.println();

  mfrc522.PCD_DumpVersionToSerial();  // Details vom PCI-MFRC522 RFID-Reader/Writer ausgeben
  Serial.println("RFID-Reader bereit zum lesen...");
}


void Read(int blockNum, byte readData[]) {
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed for Read: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Authentication success");
  }
  status = mfrc522.MIFARE_Read(blockNum, readData, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Block was read successfully");


    Serial.println("Data in Block " + String(blockNum) + ":");
    Serial.print(" -->");
    for (byte i = 0; i < 16; i++) {
      Serial.print(readData[i] < 10 & 0x10 ? " 0" : " ");
      Serial.print(readData[i], HEX);           // Umwandeln in Hexadezimal
    }
    Serial.println();

    //Checkt auf richtige Daten auf Karte
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
}


void loop() {
  // Sobald ein Chip aufgelegt wird startet diese Abfrage
  if (mfrc522.PICC_IsNewCardPresent()) {
    mfrc522.PICC_ReadCardSerial();

    chipUID = 0; // CardID resetten
    // die ID des Chips in die Variable chipUID geladen
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      chipUID = ((chipUID + mfrc522.uid.uidByte[i]) * 10);
    }

    Serial.print("RFID-Chip UID: ");
    Serial.print(chipUID);     // Card UID ausgeben
    Serial.print("  |  PICC type: ");
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);  // Modell der RFID-Karte
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    Serial.println("Reading from Data Block...");
    Read(blockNum, readData);
    Serial.println();

    //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));   // Test Dump alle Daten auf RFID-Karte
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    // Danach 3 Sekunden pausieren um mehrfaches Lesen / Ausführen zu verhindern
    byte a = 3;
    while (a > 0) {
      Serial.println("Bereit in: " + String(a) + "s");
      a--;
      delay(1000);
    }
    Serial.println("Bereit zum Lesen...\n");

  }
}
