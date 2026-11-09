#include "Arduino.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "user_define.h"
#include "robot.h"
//#include "Accelerometre.h"


Robot robot;
//Accelerometre accelerometre;


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
/*
  accelerometre.Accel_init();

  bool test = accelerometre.Error_check();

  if (test == true){
    Serial.print("Test OK");
  }
  */

  robot.initialisation(onDataRecv);
  //robot.accelerometre.Error_check();
}

void loop() {
  robot.update();
  Serial.print("Hello world");
  delay(3000);
}
