# RFID door lock

## Description

Ein 

**Achtung:**
Die Kommunikation zwischen den ESPs ist nicht verschlüsselt und die verschlüsslung der RFID Karten ist nicht sicher.

Eine sichere alternative für eine two way communication zwischen den ESPs wäre zum Beispiel [ESP-NOW](https://www.electronicwings.com/nodemcu/esp-now-two-way-communication-for-esp8266-nodemcu-)

## Features 

- **Access Point Modus:** Netzwerk einstellung werden über AP Modus eingerichtet.
- Scant RFID-Karten auf richtige Schlüssel Daten.
- Mehrere Clients möglich.
- **ESP1 Websaite:** zeigt den Status der LED an.
- **Displays:** zeigen den Status des ESP und der LED an. 

<!--
## Funktionen
Ein ESP8266(1) mit RFID-Reader und einem Display sowie zwei ESP(2) mit Display und einer LED.
Der ESP(1) mit RFID-Reader liest den RFID-Chip und sendet den RFID-Code zum ESP(2), der
entscheidet, ob dieser Code richtig definiert/eingespeichert ist oder nicht. Bei richtigem RFID-Code
wird auf beiden Displays "Entriegelt" angezeigt und die LED (ESP(2)) wird eingeschaltet. Die LED
symbolisiert die Entriegelung einer Tür/eines Türschlosses. Der entriegelte Zustand bleibt für einen
kurzen Zeitraum (5s) erhalten. Die Verbindung zwischen ESP(1) und ESP(2) erfolgt über WLAN.

### Soll Funktionen:
- Auf dem Display wird der Verarbeitungszustand angezeigt (Chip gelesen, Chip überprüfen, …)
- Der ESP(1) gibt eine Webseite aus, die den Zustand der Entriegelung angibt.
- Kommunikation über TCP/IP Protokoll

### Optionale Funktionen:
- WLAN-Konfiguration via Access-Point (Freischaltung durch Taster)
- Die WLAN-Kommunikation verschlüsseln
-->

## Items we used

- 2x ESP8266 D1 mini pro
- RFID-RC522
- 2x OLED 0.66'' 64x48
- cables
- LED
- resistance

## Connection Diagram
<img width="419" src="https://github.com/GalacticCodeGambit/Tuerschloss-RFID/assets/150372421/607786e3-a26c-4ede-8bcb-3764e9ef0c5a">

## Troubleshooting
- **The ESP1 automatically switches to access point mode after setting up the WLAN settings**
  1. Deactivate/comment out "operatingProof()" in the code.
- **The ESP1 issues the message "Error no clients connected"**
  1. Restart clients.
   

## Contributing
We welcome contributions from the community! If you have suggestions for improvements or new features, please open an issue or submit a pull request on our [GitHub repository](https://github.com/GalacticCodeGambit/RFID-door-lock).


## Helpful links
- [Data structure on RFID cards](https://arduino-projekte.webnode.at/meine-projekte/zugangskontrolle-mit-rfid/tag-speicher-lesen/#:~:text=Die%2016-Byte%20gro%C3%9Fen%20Datenb%C3%B6cke,k%C3%B6nnen%20je%20einen%20Zahlenwert%20enthalten.)
- [How to use a RFID card reader](https://www.youtube.com/watch?v=hxSQmTkIGAs)
