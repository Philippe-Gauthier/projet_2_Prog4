/*
  Ce fichier contient les déclarations des fonctions
  utilisées pour contrôler la voiture RC.
*/

#ifndef RC_CAR_H
#define RC_CAR_H

#include <stdint.h>      // Types comme uint8_t
#include "Arduino.h"     // Fonctions Arduino


// ==================== FONCTIONS ====================

// Initialise les moteurs et les périphériques
void rcCar_setup();

// Arrête les moteurs
void rcCar_stop();

// Contrôle les moteurs avec le joystick
void rcCar_cmd(int x, int y);

// Lit le niveau de la batterie
void getBatteryPercentage();

// Change la couleur selon la batterie
void updateBatteryLED(int batteryPercentage);

// Reçoit les données ESP-NOW
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

// Allume ou éteint les phares
void toggleLights();

#endif // RC_CAR_H