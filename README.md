# RFID door lock


## Description
ESP1 uses an RFID-Reader to scan the RFID card for the correct key data, if this is the case, it sends to the ESP2s (clients) that they should set the LED to high, if they respond that they have done this, changes the ESP1 on the website and the display is unlocked. After 5 seconds the ESP1 sends the clients to set the LED to low, if they respond the status on the display and the web page changes.  

**Attention:** The communication between the ESPs is not encrypted and the encryption of the RFID cards is no longer secure. <br/> A safe alternative for a two-way communication between the ESPs would be, for example [ESP-NOW](https://www.electronicwings.com/nodemcu/esp-now-two-way-communication-for-esp8266-nodemcu-).


## Features 
- **Access Point Mode:** Network settings are set up via AP mode.
- Scans RFID cards for correct key data.
- Multiple clients possible.
- **Webstring:** ESP1 displays the status of the lock on a webstring.
- **Displays:** show the status of the ESP and the lock.
- **Communication:** is via TCP/IP in your WiFi network.


## Items we used
- 2x ESP8266 D1 mini pro
- RFID-RC522
- 2x OLED 0.66'' 64x48
- cables
- LED
- resistance


## Connection Diagram
<img width="419" src="https://github.com/GalacticCodeGambit/Tuerschloss-RFID/assets/150372421/607786e3-a26c-4ede-8bcb-3764e9ef0c5a">
<img width="419" src="https://github.com/GalacticCodeGambit/RFID-door-lock/assets/150372421/c330ce8e-0be8-4b24-92a6-b4a76b7c442a">


## Troubleshooting
- **The ESP1 automatically switches to access point mode after setting up the WLAN settings**
  1. Deactivate/comment out "operatingProof()" in the code.
- **The ESP1 issues the message "Error no clients connected"**
  1. Restart clients.
### Working library versions
- EEPROM-Rotate 0.9.2
- MFRC522 1.4.11
- Adafruit GFX Library 1.11.9
- Adafruit SSD1306 Wemos Mini OLED 1.1.2
- WiFi 1.2.7
   

## Contributing
We welcome contributions from the community! If you have suggestions for improvements or new features, please open an issue or submit a pull request on our [GitHub repository](https://github.com/GalacticCodeGambit/RFID-door-lock).


## Helpful links
- [Data structure on RFID cards](https://arduino-projekte.webnode.at/meine-projekte/zugangskontrolle-mit-rfid/tag-speicher-lesen/#:~:text=Die%2016-Byte%20gro%C3%9Fen%20Datenb%C3%B6cke,k%C3%B6nnen%20je%20einen%20Zahlenwert%20enthalten.)
- [How to use a RFID card reader](https://www.youtube.com/watch?v=hxSQmTkIGAs)
