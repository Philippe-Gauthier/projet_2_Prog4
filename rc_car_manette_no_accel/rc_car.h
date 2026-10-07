// Initialization des fonctions utilisées dans le code

#ifndef RC_CAR_H
#define RC_CAR_H

// Empêche ce fichier d'être inclus plusieurs fois dans le programme


#include <stdint.h>   // Permet d'utiliser des types comme uint8_t
#include "Arduino.h"  // Fonctions et types de base Arduino


// ==================== DÉCLARATION DES FONCTIONS ====================


// -------- Contrôle des moteurs --------

// Initialise les moteurs et leur PWM
void rcCar_setup();

// Arrête les deux moteurs
void rcCar_stop();

// Contrôle les moteurs avec les valeurs X et Y du joystick
void rcCar_cmd(int x, int y);

// Initialization des fonctions pour indiquer le pourcentage de la batterie
// Battery Monitoring Function
void getBatteryPercentage();

// Change la couleur du NeoPixel selon le niveau de batterie
void updateBatteryLED(int batteryPercentage);


// -------- Communication ESP-NOW et lumières --------

// Fonction appelée lorsqu'un message ESP-NOW est reçu
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

// Allume ou éteint les phares
void toggleLights();


#endif // RC_CAR_H