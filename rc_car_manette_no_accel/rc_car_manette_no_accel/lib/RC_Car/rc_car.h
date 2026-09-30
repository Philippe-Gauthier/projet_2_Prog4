#ifndef RC_CAR_H
#define RC_CAR_H

#include <stdint.h>
#include "Arduino.h"

// ==================== FUNCTION DECLARATIONS ====================

// Motor Control Functions
void rcCar_setup();
void rcCar_stop();
void rcCar_cmd(int x, int y);

// Battery Monitoring Function
void getBatteryPercentage();
void updateBatteryLED(int batteryPercentage);

// ESP-NOW Communication Functions
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
void toggleLights();

#endif // RC_CAR_H
