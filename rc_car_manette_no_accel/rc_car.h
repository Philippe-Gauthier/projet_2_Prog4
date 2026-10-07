// Initialization des fonctions utilisées dans le code

#ifndef RC_CAR_H
#define RC_CAR_H

#include <stdint.h>
#include "Arduino.h"

// ==================== FUNCTION DECLARATIONS ====================

// Initializations des fonctions pour controler et initialiser les moteurs
// Motor Control Functions
void rcCar_setup();
void rcCar_stop();
void rcCar_cmd(int x, int y);

// Initialization des fonctions pour indiquer le pourcentage de la batterie
// Battery Monitoring Function
void getBatteryPercentage();
void updateBatteryLED(int batteryPercentage);

// Initialization des fonctions pour recevoir et indiquer la réception d'informations du ESP
// ESP-NOW Communication Functions
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
void toggleLights();

#endif // RC_CAR_H
