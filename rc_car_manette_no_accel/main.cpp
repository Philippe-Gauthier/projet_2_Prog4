/*
  Voiture RC avec ESP32
  - Contrôle de 2 moteurs
  - Communication ESP-NOW
  - Détection d'inclinaison avec IMU
  - NeoPixel pour la batterie
  - Surveillance de la batterie
*/

// ==================== BIBLIOTHÈQUES ====================

#include "Arduino.h"              // Fonctions de base Arduino
#include "soc/soc.h"              // Fonctions bas niveau du ESP32
#include "soc/rtc_cntl_reg.h"     // Registres de contrôle du ESP32

#include <esp_now.h>              // Communication sans fil ESP-NOW
#include <WiFi.h>                 // Wi-Fi nécessaire pour ESP-NOW
#include <Wire.h>                 // Communication I2C

#include <ArduinoJson.h>          // Lecture des commandes au format JSON
#include "Adafruit_NeoPixel.h"    // Contrôle de la DEL NeoPixel

#include "user_define.h"          // Broches et constantes du projet
#include "rc_car.h"               // Fonctions de contrôle de la voiture


// ==================== VARIABLES GLOBALES ====================

// État des lumières et de l'IMU
bool LED_STATE = false;           // false = phares éteints
bool IMU_ERROR = true;            // true = erreur IMU au départ

// État précédent et actuel du bouton SELECT
bool previousSeState = true;
bool currentSeState = true;

// Sécurité d'inclinaison
bool tiltDetected = false;        // true = inclinaison détectée

// Temps pour les lectures de l'IMU
static unsigned long lastIMUReading = 0;  // Temps de la dernière lecture
static unsigned long IMUInterval = 50;    // Intervalle de 50 ms

// Temps pour la vérification de la batterie
static unsigned long lastBatteryCheck = 0;             // Dernière vérification
static unsigned long BatteryCheckInterval = 30000;     // 30 secondes

// ==================== NEOPIXEL ====================

// Création du NeoPixel utilisé pour afficher l'état de la batterie
Adafruit_NeoPixel pixelsBattery(NEOPIXEL_BATTERY_NUMBER, NEOPIXEL_BATTERY_PIN, NEO_GRB + NEO_KHZ800);

// ==================== STRUCTURE DES DONNÉES ====================

// Structure contenant la commande JSON reçue par ESP-NOW
typedef struct struct_message {
  char command[256];              // Tableau qui contient la commande reçue
} struct_message;

struct_message incomingMessage;   // Variable qui recevra le message

// ==================== APPAREIL DISTANT ====================

// Adresse MAC de la télécommande
uint8_t peerAddress[] = {0xDC, 0x54, 0x75, 0xC0, 0x83, 0xCC};

// ==================== SETUP ====================

// setup() est exécuté une seule fois au démarrage
void setup() {

  // Désactive la protection brownout pour éviter certains redémarrages
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  // Initialise le contrôle des moteurs
  rcCar_setup();


  // Initialise le NeoPixel de la batterie
  pixelsBattery.begin();
  pixelsBattery.setBrightness(NEOPIXEL_BRIGHTNESS);  // Règle la luminosité
  pixelsBattery.fill(0x0000FF);                      // Bleu au démarrage
  pixelsBattery.show();                              // Affiche la couleur


  // Démarre la communication série à 115200 bauds
  Serial.begin(115200);

  if (DEBUG) {
    Serial.setDebugOutput(true);     // Active les messages de debug
    //while(!Serial); // Attendrait la connexion du port série
    delay(3000);                     // Attend 3 secondes
  } else {
    Serial.setDebugOutput(false);    // Désactive les messages de debug
  }
        

  // ==================== INITIALISATION ESP-NOW ====================
  
  // Place le Wi-Fi en mode Station pour utiliser ESP-NOW
  WiFi.mode(WIFI_STA);

  Serial.print("ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());  // Affiche l'adresse MAC du ESP32
  

  // Initialise ESP-NOW
  if (esp_now_init() != ESP_OK) {

    // Si l'initialisation échoue
    Serial.println("Error initializing ESP-NOW");

    pixelsBattery.fill(0x0000FF);
    pixelsBattery.show();

    delay(2000);
    ESP.restart();                    // Redémarre le ESP32
    return;                           // Quitte setup()
  }
  

  // Appelle onDataRecv() automatiquement lorsqu'un message est reçu
  esp_now_register_recv_cb(onDataRecv);


  // ==================== CONFIGURATION DE LA TÉLÉCOMMANDE ====================
  
  // Crée la configuration de l'appareil distant
  esp_now_peer_info_t peerInfo = {};

  // Copie l'adresse MAC dans la configuration
  memcpy(peerInfo.peer_addr, peerAddress, 6);

  peerInfo.channel = 0;               // Canal de communication
  peerInfo.encrypt = false;           // Communication non chiffrée
  peerInfo.ifidx = WIFI_IF_STA;       // Utilise le mode Station


  // Affiche l'adresse MAC de la télécommande
  Serial.print("Adding peer with MAC: ");

  for (int i = 0; i < 6; i++) {

      // Affiche chaque partie de l'adresse MAC en hexadécimal
      Serial.printf("%02X", peerAddress[i]);

      // Ajoute ":" entre les parties de l'adresse
      if (i < 5) Serial.print(":");
  }

  Serial.println();


  // Vérifie si la télécommande est déjà enregistrée
  if (!esp_now_is_peer_exist(peerAddress)) {

      // Ajoute la télécommande à ESP-NOW
      if (esp_now_add_peer(&peerInfo) != ESP_OK) {

          // Si l'ajout échoue
          Serial.println("Failed to add peer");

          pixelsBattery.fill(0x0000FF);
          pixelsBattery.show();

          delay(2000);
          ESP.restart();
          return;
      }

  } else {

      // La télécommande était déjà enregistrée
      Serial.println("Peer already exists");
  }

  // ==================== FIN DU DÉMARRAGE ===================

  // Effectue une première vérification de la batterie
  Serial.println("Performing initial battery check...");
  getBatteryPercentage();


  Serial.println("Setup complete, entering main loop...");


  // Change l'état des phares 6 fois = 3 clignotements
  for (int i = 0; i < 6; i++) {
    toggleLights();
    delay(100);                       // 100 ms entre chaque changement
  }
}

// ==================== BOUCLE PRINCIPALE ====================

// loop() est répétée continuellement
void loop() {

  // Vérifie si 30 secondes se sont écoulées
  if (millis() - lastBatteryCheck > BatteryCheckInterval) {

    // Enregistre le moment de la vérification
    lastBatteryCheck = millis();

    // Vérifie le niveau de la batterie
    getBatteryPercentage();
  }
}