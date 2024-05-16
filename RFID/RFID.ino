#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  15     // Pin: D8  15
#define RST_PIN  0     // Pin: D3  0

long chipUID;

MFRC522 mfrc522(SS_PIN, RST_PIN); // MFRC522-Instanz erstellen
MFRC522::MIFARE_Key key;

int d = 300;
int blockNum = 2;
byte bufferLen = 18;
byte readData[18];

MFRC522::StatusCode status;


void setup() {
  Serial.begin(9600);
  SPI.begin();  // SPI-Bus initialisieren
  mfrc522.PCD_Init();  //Startet RFID Sensor
  delay(4);

  Serial.println("");

  mfrc522.PCD_DumpVersionToSerial();  // Details vom PCI - MFRC522 RFID READER / WRITER ausgeben
  Serial.println("RFID-Reader bereit zum lesen...");
}


void Read(int blockNum, byte readData[]) {
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed for Read: ");
    delay(d);
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Authentication success");
    delay(d);
  }
  status = mfrc522.MIFARE_Read(blockNum, readData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    delay(d);
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Block was read successfully");
    delay(d);
  }
}


void loop() {

  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  // Sobald ein Chip aufgelegt wird startet diese Abfrage
  if (mfrc522.PICC_IsNewCardPresent()) {
    chipUID = 0; // CardID resetten

    mfrc522.PICC_ReadCardSerial();

    // die ID des Chips in die Variable chipUID geladen
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      chipUID = ((chipUID + mfrc522.uid.uidByte[i]) * 10);
    }

    Serial.print("RFID-Chip UID: "); //Card UID
    Serial.print(chipUID);  // UID ausgeben
    Serial.print("  |  PICC type: ");
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);  // Modell des RFID Senors
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    Serial.println("Reading from Data Block...");
    // mfrc522.PICC_DumpToSerial(&(mfrc522.uid));   //Test Dump alle Daten
    //Read(blockNum, readData);

    //--------------------//
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

    if (status != MFRC522::STATUS_OK) {
      Serial.print("Authentication failed for Read: ");
      delay(d);
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    } else {
      Serial.println("Authentication success");
      delay(d);
    }
    status = mfrc522.MIFARE_Read(blockNum, readData, &bufferLen);
    if (status != MFRC522::STATUS_OK) {
      Serial.print("Reading failed: ");
      delay(d);
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    } else {
      Serial.println("Block was read successfully");
      delay(d);
    }
    //--------------------//
    if (status == MFRC522::STATUS_OK) {
      Serial.println("Data in Block:");
      Serial.print(" --> ");
      for (int i = 0; i < 16; i++) {  //uint8_t
        Serial.print(readData[i]);
      }
    } else {
      Serial.print("Reading failed");
    }
    Serial.print("\n");

    
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
    

    delay(5000);  // Danach 5 Sekunden pausieren um mehrfaches Lesen / Ausführen zu verhindern
    Serial.println("Bereit zum Lesen...\n");
  }
}
