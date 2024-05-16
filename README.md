# Tuerschloss-RFID
Ein ESP8266(1) mit RFID-Reader und einem Display sowie zwei ESP(2) mit Display und einer LED.
Der ESP(1) mit RFID-Reader liest den RFID-Chip und sendet den RFID-Code zum ESP(2), der
entscheidet, ob dieser Code richtig definiert/eingespeichert ist oder nicht. Bei richtigem RFID-Code
wird auf beiden Displays "Entriegelt" angezeigt und die LED (ESP(2)) wird eingeschaltet. Die LED
symbolisiert die Entriegelung einer Tür/eines Türschlosses. Der entriegelte Zustand bleibt für einen
kurzen Zeitraum (5s) erhalten. Die Verbindung zwischen ESP(1) und ESP(2) erfolgt über WLAN.
Soll Funktionen:
- Auf dem Display wird der Verarbeitungszustand angezeigt (Chip gelesen, Chip überprüfen, …)
- Der ESP(1) gibt eine Webseite aus, die den Zustand der Entriegelung angibt.
- Kommunikation über TCP/IP Protokoll
Optionale Funktionen:
- WLAN-Konfiguration via Access-Point (Freischaltung durch Taster)
- Die WLAN-Kommunikation verschlüsseln
