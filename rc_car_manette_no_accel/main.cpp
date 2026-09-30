/*
  Ce programme :
  - Initialise la voiture RC
  - Initialise ESP-NOW
  - Reçoit les commandes de la manette
  - Contrôle les NeoPixel
  - Vérifie la batterie
  - Prépare la détection d'inclinaison avec l'IMU

  Auteur original : LP Gauthier - 2025
*/


// ==================== BIBLIOTHÈQUES ====================

#include "Arduino.h"              // Fonctions Arduino
#include "soc/soc.h"              // Fonctions internes ESP32
#include "soc/rtc_cntl_reg.h"     // Gestion de l'alimentation ESP32

#include <esp_now.h>              // Communication ESP-NOW
#include <WiFi.h>                 // WiFi ESP32
#include <Wire.h>                 // Communication I2C

#include <ArduinoJson.h>          // Lecture des messages JSON
#include "Adafruit_NeoPixel.h"    // Contrôle des NeoPixel

#include "user_define.h"          // Constantes et broches du projet
#include "rc_car.h"               // Fonctions de la voiture


// ==================== VARIABLES GLOBALES ====================

// État des lumières
bool LED_STATE = false;

// État de l'IMU
bool IMU_ERROR = true;

// État précédent du bouton SELECT
bool previousSeState = true;

// État actuel du bouton SELECT
bool currentSeState = true;

// Indique si la voiture est trop inclinée
bool tiltDetected = false;


// Dernière lecture de l'IMU
static unsigned long lastIMUReading = 0;

// Lecture IMU toutes les 50 ms
static unsigned long IMUInterval = 50;


// Dernière vérification de batterie
static unsigned long lastBatteryCheck = 0;

// Vérification batterie toutes les 30 secondes
static unsigned long BatteryCheckInterval = 30000;


// ==================== NEOPIXEL ====================

// NeoPixel utilisé pour afficher l'état de la batterie
Adafruit_NeoPixel pixelsBattery(
  NEOPIXEL_BATTERY_NUMBER,
  NEOPIXEL_BATTERY_PIN,
  NEO_GRB + NEO_KHZ800
);


// ==================== MESSAGE ESP-NOW ====================

// Structure utilisée pour recevoir les commandes
typedef struct struct_message {

  // Contient le message JSON reçu
  char command[256];

} struct_message;


// Message reçu
struct_message incomingMessage;


// ==================== MANETTE ====================

// Adresse MAC de la manette
uint8_t peerAddress[] = {
  0xDC,
  0x54,
  0x75,
  0xC0,
  0x83,
  0xCC
};


// ==================== SETUP ====================

/*
  setup()
  Initialise les moteurs, les NeoPixel,
  le WiFi et ESP-NOW.
*/
void setup() {

  // Désactive la protection contre les baisses de tension
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // Initialise les moteurs
  rcCar_setup();


  // Initialise les NeoPixel
  pixelsBattery.begin();

  // Règle la luminosité
  pixelsBattery.setBrightness(NEOPIXEL_BRIGHTNESS);

  // Met les NeoPixel en bleu
  pixelsBattery.fill(0x0000FF);

  // Affiche la couleur
  pixelsBattery.show();


  // Initialise le port série
  Serial.begin(115200);


  // Vérifie si le mode DEBUG est activé
  if (DEBUG) {

    // Active les messages de debug
    Serial.setDebugOutput(true);

    // Attend 3 secondes
    delay(3000);

  } else {

    // Désactive les messages de debug
    Serial.setDebugOutput(false);
  }


  // ==================== ESP-NOW ====================

  // Met le WiFi en mode station
  WiFi.mode(WIFI_STA);

  // Affiche l'adresse MAC de l'ESP32
  Serial.print("ESP32 MAC Address: ");

  // Affiche l'adresse MAC
  Serial.println(WiFi.macAddress());


  // Initialise ESP-NOW
  if (esp_now_init() != ESP_OK) {

    // Affiche une erreur
    Serial.println("Error initializing ESP-NOW");

    // Met le NeoPixel en bleu
    pixelsBattery.fill(0x0000FF);

    // Affiche la couleur
    pixelsBattery.show();

    // Attend 2 secondes
    delay(2000);

    // Redémarre l'ESP32
    ESP.restart();

    // Quitte setup()
    return;
  }


  // Appelle onDataRecv() quand un message est reçu
  esp_now_register_recv_cb(onDataRecv);


  // ==================== AJOUT DE LA MANETTE ====================

  // Crée les informations de la manette
  esp_now_peer_info_t peerInfo = {};

  // Copie l'adresse MAC de la manette
  memcpy(peerInfo.peer_addr, peerAddress, 6);

  // Utilise le canal actuel
  peerInfo.channel = 0;

  // Désactive le chiffrement
  peerInfo.encrypt = false;

  // Utilise le mode WiFi station
  peerInfo.ifidx = WIFI_IF_STA;


  // Affiche l'adresse MAC de la manette
  Serial.print("Adding peer with MAC: ");


  // Parcourt les 6 parties de l'adresse MAC
  for (int i = 0; i < 6; i++) {

    // Affiche une partie de l'adresse MAC
    Serial.printf("%02X", peerAddress[i]);

    // Ajoute ":" entre les parties
    if (i < 5)
      Serial.print(":");
  }


  // Passe à la ligne suivante
  Serial.println();


  // Vérifie si la manette est déjà ajoutée
  if (!esp_now_is_peer_exist(peerAddress)) {

    // Essaie d'ajouter la manette
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {

      // Affiche une erreur
      Serial.println("Failed to add peer");

      // Met le NeoPixel en bleu
      pixelsBattery.fill(0x0000FF);

      // Affiche la couleur
      pixelsBattery.show();

      // Attend 2 secondes
      delay(2000);

      // Redémarre l'ESP32
      ESP.restart();

      // Quitte setup()
      return;
    }

  } else {

    // Indique que la manette existe déjà
    Serial.println("Peer already exists");
  }


  // ==================== FIN DU DÉMARRAGE ====================

  // Affiche un message
  Serial.println("Performing initial battery check...");

  // Vérifie la batterie
  getBatteryPercentage();


  // Indique que l'initialisation est terminée
  Serial.println("Setup complete, entering main loop...");


  // Fait clignoter les lumières 3 fois
  for (int i = 0; i < 6; i++) {

    // Change l'état des lumières
    toggleLights();

    // Attend 100 ms
    delay(100);
  }
}


// ==================== LOOP ====================

/*
  loop()
  Vérifie la batterie toutes les 30 secondes.
*/
void loop() {

  // Vérifie si 30 secondes sont passées
  if (millis() - lastBatteryCheck > BatteryCheckInterval) {

    // Sauvegarde le temps actuel
    lastBatteryCheck = millis();

    // Vérifie la batterie
    getBatteryPercentage();
  }
}