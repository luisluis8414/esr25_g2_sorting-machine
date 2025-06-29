# Sortiermaschinen-Projekt

Dieses Projekt implementiert eine automatisierte Sortiermaschine unter Verwendung des MSP430FR2355. Das System wurde entwickelt, um Objekte (Medikamente) basierend auf ihrer Farbe mithilfe eines Farbsensors und einer servogesteuertern Kippplatform zu sortieren.

# Demo

Hier sind einige Bilder des fertigen Systems:

| <img src="docs/images/demo_system.jpg" width="250" alt="Gesamtsystem"/> | <img src="docs/images/demo_display.jpg" width="250" alt="LCD Display"/> | <img src="docs/images/demo_sort.jpg" width="250" alt="Sortiersystem"/> |
|:---:|:---:|:---:|
| *Komplettes System* | *LCD-Display - Hier Auswahl des Sortiermodus* | *Sortierplattform sortiert rote Pille* |

# Systemarchitektur

![Systemarchitektur](docs/images/UML_Einsatzdiagramm.png)

Das System besteht aus folgenden Hauptkomponenten:

- **MSP430FR2355 Launchpad**: Mikrocontroller
- **TCS34725 Farbsensor**: RGB-Farberkennung zur Objektidentifikation
- **PCA9685 Servotreiber**: PWM-Controller für Servomotoren
- **LCD1602 Display**: Statusanzeige für System-Feedback
- **Servomotoren**: Aktoren für die Sortierplattform

# Hardware-Implementierung

![Verdrahtungsplan](docs/images/wire_diagram.png)

## Hauptkomponenten und Verbindungen

- **I2C-Bus-Kommunikation**:
  - Verbindet den Farbsensor (TCS34725)
  - Steuert den Servotreiber (PCA9685)
  - Kommuniziert mit dem LCD-Display (LCD1602)
- **Servo-Plattform**: Mechanischer Sortiermechanismus, gesteuert durch Servomotoren
- **Stromversorgung**: 3.3V-Stromverteilung für alle Komponenten, 5V extra Versorgung für die Sevos

# Mikrocontroller-Peripherie

![Verwendete Peripherie](docs/images/used_peripherals.png)

Der MSP430FR2355 nutzt folgende wichtige Peripheriekomponenten:

- **I2C-Schnittstelle**: Für die Kommunikation mit TCS, PCA und LCD
- **Timer-Module**: Für Timer, Systemtick und button debouncing
- **GPIO-Ports**: Für allgemeine Ein-/Ausgabesteuerung
- **Taktsystem**: 1MHz Systemtakt

# Projektstruktur

```
esr25_g2_sorting-machine/
├── button/             - Button-Schnittstellenimplementierung
├── I2C/                - I2C-Kommunikationsprotokoll
├── lcd1602_display/    - LCD-Display-Treiber und Manager
├── led/                - LED-Steuerungsimplementierung
├── PCA9685/            - Servotreiber-Controller
├── platform/           - Plattform-Steuerungslogik
├── state_machine/      - Hauptsystem-Zustandsverwaltung
├── TCS34725/           - Farbsensor-Treiber
└── timer/              - Timer-Konfigurationen
```

# Dokumentation generieren

Das Projekt verwendet Doxygen zur Dokumentationsgenerierung. Um die Dokumentation zu erstellen:

1. Stellen Sie sicher, dass [Doxygen](https://www.doxygen.nl/download.html) installiert ist
2. Führen Sie das Script `generate_docs.bat` aus
3. Die generierte Dokumentation finden Sie unter `docs/html/index.html`

Das Script wird automatisch:
- Die Doxygen Awesome CSS Theme herunterladen (falls nicht vorhanden)
- Die alte Dokumentation löschen
- Neue Dokumentation generieren
- Die Dokumentation im Browser öffnen

Die Dokumentation enthält:
- Detaillierte API-Beschreibungen
- Funktionsreferenzen
- Architekturdiagramme
- Hardwareverbindungen
- Beispielbilder des Systems
