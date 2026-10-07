/*
  Voiture RC avec ESP32
  Le programme initialise la voiture, la communication ESP-NOW,
  le NeoPixel et la surveillance de la batterie.
*/


// ==================== BIBLIOTHÈQUES ====================

#include "Arduino.h"              // Fonctions de base Arduino
#include "soc/soc.h"              // Fonctions internes du ESP32
#include "soc/rtc_cntl_reg.h"     // Contrôle de certains registres du ESP32

#include <esp_now.h>              // Communication sans fil ESP-NOW
#include <WiFi.h>                 // Wi-Fi nécessaire pour ESP-NOW
#include <Wire.h>                 // Communication I2C

#include <ArduinoJson.h>          // Permet de traiter les messages JSON
#include "Adafruit_NeoPixel.h"    // Contrôle du NeoPixel

#include "user_define.h"          // Broches et constantes du projet
#include "rc_car.h"               // Fonctions de contrôle de la voiture


// ==================== VARIABLES GLOBALES ====================

// État des phares et de l'IMU
bool LED_STATE = false;
bool IMU_ERROR = true;

// Permet de détecter un changement d'état du bouton SELECT
bool previousSeState = true;
bool currentSeState = true;

// Indique si une inclinaison a été détectée
bool tiltDetected = false;

// Variables de temps pour l'IMU
static unsigned long lastIMUReading = 0;
static unsigned long IMUInterval = 50; // 50 ms

// Variables de temps pour vérifier la batterie
static unsigned long lastBatteryCheck = 0;
static unsigned long BatteryCheckInterval = 30000; // 30 secondes


// ==================== NEOPIXEL ====================

// NeoPixel utilisé comme indicateur de batterie
Adafruit_NeoPixel pixelsBattery(NEOPIXEL_BATTERY_NUMBER, NEOPIXEL_BATTERY_PIN, NEO_GRB + NEO_KHZ800);


// ==================== STRUCTURE DU MESSAGE ====================

// Structure qui contient la commande JSON reçue par ESP-NOW
typedef struct struct_message {
  char command[256];
} struct_message;

struct_message incomingMessage;


// ==================== TÉLÉCOMMANDE ====================

// Adresse MAC de l'appareil avec lequel le ESP32 communique
uint8_t peerAddress[] = {0xDC, 0x54, 0x75, 0xC0, 0x83, 0xCC};


// ==================== SETUP ====================

// setup() est exécuté une seule fois au démarrage
void setup() {

  // Désactive la protection brownout
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  // Initialise les moteurs et les éléments de la voiture
  rcCar_setup();


  // Initialise le NeoPixel
  pixelsBattery.begin();
  pixelsBattery.setBrightness(NEOPIXEL_BRIGHTNESS);
  pixelsBattery.fill(0x0000FF); // Bleu au démarrage
  pixelsBattery.show();


  // Démarre la communication série
  Serial.begin(115200);

  if (DEBUG) {
    Serial.setDebugOutput(true);
    //while(!Serial);
    delay(3000);
  } else {
    Serial.setDebugOutput(false);
  }
        

  // ==================== ESP-NOW ====================
  
  // Place le Wi-Fi en mode station
  WiFi.mode(WIFI_STA);

  // Affiche l'adresse MAC du ESP32
  Serial.print("ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());
  

  // Initialise ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");

    pixelsBattery.fill(0x0000FF);
    pixelsBattery.show();

    delay(2000);

    // Redémarre le ESP32 si ESP-NOW ne fonctionne pas
    ESP.restart();
    return;
  }
  

  // onDataRecv() sera appelée lorsqu'un message ESP-NOW est reçu
  esp_now_register_recv_cb(onDataRecv);


  // ==================== APPAREIL DISTANT ====================
  
  // Crée les informations de l'appareil distant
  esp_now_peer_info_t peerInfo = {};

  // Copie son adresse MAC
  memcpy(peerInfo.peer_addr, peerAddress, 6);

  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA;


  // Affiche son adresse MAC dans le moniteur série
  Serial.print("Adding peer with MAC: ");

  for (int i = 0; i < 6; i++) {
      Serial.printf("%02X", peerAddress[i]);

      if (i < 5) Serial.print(":");
  }

  Serial.println();


  // Ajoute l'appareil s'il n'est pas déjà enregistré
  if (!esp_now_is_peer_exist(peerAddress)) {

      if (esp_now_add_peer(&peerInfo) != ESP_OK) {
          Serial.println("Failed to add peer");

          pixelsBattery.fill(0x0000FF);
          pixelsBattery.show();

          delay(2000);
          ESP.restart();
          return;
      }

  } else {
      Serial.println("Peer already exists");
  }


  // ==================== FIN DU DÉMARRAGE ====================

  // Vérifie une première fois le niveau de la batterie
  Serial.println("Performing initial battery check...");
  getBatteryPercentage();


  Serial.println("Setup complete, entering main loop...");


  // Change l'état des phares 6 fois, ce qui donne 3 clignotements
  for (int i = 0; i < 6; i++) {
    toggleLights();
    delay(100);
  }
}


// ==================== BOUCLE PRINCIPALE ====================

// loop() fonctionne continuellement après le setup
void loop() {

  // Vérifie la batterie toutes les 30 secondes
  if (millis() - lastBatteryCheck > BatteryCheckInterval) {

    // Mémorise le moment de la dernière vérification
    lastBatteryCheck = millis();

    // Lit le niveau de la batterie
    getBatteryPercentage();
  }
}