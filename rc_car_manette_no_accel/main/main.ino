#include "Arduino.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "user_define.h"
#include "robot.h"

robot robot;

void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  robot.reception_commande(incomingData, len);
}

void setup() {

  // Initialize serial communication (115200 baud)
  Serial.begin(115200);
  if (DEBUG) {
    Serial.setDebugOutput(true);
    delay(3000);
  } else {
    Serial.setDebugOutput(false);
  }


  robot.initialisation(onDataRecv);
}

void loop() {
  robot.update();
}