# RFID door lock


## Description
ESP1 scannt mit einem RFID-Scanner die RFID-Karte auf die richtigen Schlüsseldaten, wenn das der Fall ist, sendet er an die ESP2s(Clients) das sie die LED auf high stellen sollen, wenn sie antworten, dass sie das gemacht haben, ändert der ESP1 auf der Webseite und dem Display auf entriegelt. Nach 5 Sekunden sendet der ESP1 den Clients, dass sie die LED auf low stellen sollen, wenn sie antworten, ändert sich der Status auf dem Display und der Websaite.  

**Achtung:** Die Kommunikation zwischen den ESPs ist nicht verschlüsselt und die Verschlüsselung der RFID-Karten ist nicht mehr sicher.

Eine sichere Alternative für eine two way communication zwischen den ESPs wäre zum Beispiel [ESP-NOW](https://www.electronicwings.com/nodemcu/esp-now-two-way-communication-for-esp8266-nodemcu-)


## Features 
- **Access Point Modus:** Netzwerkeinstellungen werden über AP-Modus eingerichtet.
- Scannt RFID-Karten auf richtige Schlüsseldaten.
- Mehrere Clients möglich.
- **Websaite:** ESP1 zeigt den Status des Schlosses auf einer Websaite an.
- **Displays:** zeigen den Status des ESP und des Schlosses an.
- **Kommunikation:** ist über TCP/IP. 


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
