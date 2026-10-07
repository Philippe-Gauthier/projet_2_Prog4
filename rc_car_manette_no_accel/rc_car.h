#ifndef RC_CAR_H
#define RC_CAR_H

// Empêche le fichier d'être inclus plusieurs fois


#include <stdint.h>   // Permet d'utiliser des types comme uint8_t
#include "Arduino.h"  // Fonctions de base Arduino


// ==================== DÉCLARATION DES FONCTIONS ====================


// Initialise les moteurs et les PWM
void rcCar_setup();

// Arrête les deux moteurs
void rcCar_stop();

// Contrôle la vitesse et la direction des moteurs avec le joystick
void rcCar_cmd(int x, int y);


// Lit la batterie et calcule son pourcentage
void getBatteryPercentage();

// Change la couleur du NeoPixel selon le niveau de batterie
void updateBatteryLED(int batteryPercentage);


// Fonction appelée lorsqu'un message ESP-NOW est reçu
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

// Allume ou éteint les phares
void toggleLights();


#endif // RC_CAR_H