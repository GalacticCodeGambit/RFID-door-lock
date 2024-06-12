# RFID-door-lock

## Items we used
- 2x ESP8266 D1 mini pro
- RFID-RC522
- 2x OLED 0.66'' 64x48
- cables
- LED
- resistance

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

## Connection Diagram
<img width="419" src="https://github.com/GalacticCodeGambit/Tuerschloss-RFID/assets/150372421/607786e3-a26c-4ede-8bcb-3764e9ef0c5a">

## Troubleshooting
* **If the ESP1 automatically switches to access point mode after setting up the WLAN settings**
  1. deactivate/comment out "operatingProof()" in the code
* **If the ESP1 issues the message "Error no clients connected"**
  1. restart clients
   

## Contributing
We welcome contributions from the community! If you have suggestions for improvements or new features, please open an issue or submit a pull request on our [GitHub repository](https://github.com/GalacticCodeGambit/RFID-door-lock).
