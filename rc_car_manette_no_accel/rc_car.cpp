#include "rc_car.h"
#include "Arduino.h"
#include "driver/mcpwm.h"         // Contrôle PWM des moteurs
#include "Adafruit_NeoPixel.h"   // Contrôle du NeoPixel
#include <ArduinoJson.h>          // Lecture des messages JSON
#include <Wire.h>                 // Communication I2C
#include "user_define.h"          // Broches et constantes du projet


// ==================== PHARES ====================

// Canal PWM utilisé pour contrôler les phares
const int lights_pwm_channel = 0;


// ==================== VARIABLES EXTERNES ====================

// Variables créées dans le main.cpp et utilisées dans ce fichier
extern bool LED_STATE;
extern bool previousSeState;
extern bool currentSeState;
extern bool tiltDetected;
extern Adafruit_NeoPixel pixelsBattery;


// Structure du message reçu par ESP-NOW
typedef struct struct_message {
  char command[256];
} struct_message;

extern struct_message incomingMessage;


// Puissance des phares, haute puissance par défaut
int lights_power = HIGH_BEAM_POWER;


// Convertit un pourcentage de 0-100 en valeur PWM de 0-255
static uint8_t percentToPWM(int percent) {
  percent = constrain(percent, 0, 100);
  return (uint8_t)map(percent, 0, 100, 0, 255);
}


// ==================== INITIALISATION ====================

void rcCar_setup()
{
  // Configure les broches du moteur droit
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, RIGHT_MOTOR_FWD);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, RIGHT_MOTOR_BWD);
  
  // Configure les broches du moteur gauche
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, LEFT_MOTOR_FWD);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, LEFT_MOTOR_BWD);


  // Configuration du PWM des moteurs
  mcpwm_config_t pwm_config;
  pwm_config.frequency = 5000;              // Fréquence de 5 kHz
  pwm_config.cmpr_a = 0;                    // Rapport cyclique à 0 % au départ
  pwm_config.cmpr_b = 0;
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;


  // Timer 0 pour le moteur droit et Timer 1 pour le moteur gauche
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);


  // Arrête les moteurs au démarrage
  rcCar_stop();


  // Configure le PWM des phares
  ledcSetup(lights_pwm_channel, 5000, 8);
  ledcAttachPin(LIGHTS_PIN, lights_pwm_channel);
  ledcWrite(lights_pwm_channel, 0);


  // Initialise le NeoPixel
  pixelsBattery.begin();
  pixelsBattery.setBrightness(255);
  pixelsBattery.fill(0x0000FF);      // Bleu au démarrage
  pixelsBattery.show();
}


// ==================== ARRÊT DES MOTEURS ====================

void rcCar_stop(){

  Serial.println("Stopping motors");

  // Met toutes les sorties des moteurs à 0 %
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 0);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, 0);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, 0);
}


// ==================== BATTERIE ====================

// Change la couleur du NeoPixel selon le niveau de batterie
void updateBatteryLED(int batteryPercentage) {

  if(batteryPercentage < BATTERY_RED_THRESHOLD) {

    Serial.println("Battery low, setting LED to red");
    pixelsBattery.fill(0xFF0000);         // Rouge = batterie faible

  } else if (batteryPercentage < BATTERY_YELLOW_THRESHOLD) {

    Serial.println("Battery medium, setting LED to yellow");
    pixelsBattery.fill(0xFFFF00);         // Jaune = batterie moyenne

  } else {

    Serial.println("Battery good, setting LED to green");
    pixelsBattery.fill(0x00FF00);         // Vert = batterie bonne
  }

  pixelsBattery.show();
}


// Lit la tension de la batterie et calcule son pourcentage
void getBatteryPercentage() {

  Serial.println("-------------- Reading battery voltage --------------");

  // Convertit la lecture ADC en tension
  float voltage = analogRead(ADC_BATTERY_PIN) / 4095.0f * 3.3f * 2 + 0.24f;

  Serial.print("Voltage: ");
  Serial.println(voltage);
  

  // Convertit la tension en pourcentage de 0 à 100 %
  int batteryPercentage = map(voltage * 1000, MIN_VOLTAGE * 1000, MAX_VOLTAGE * 1000, 0, 100);

  // Limite le résultat entre 0 et 100 %
  if (batteryPercentage > 100) batteryPercentage = 100;
  if (batteryPercentage < 0) batteryPercentage = 0;


  Serial.print("Battery Percentage: ");
  Serial.println(batteryPercentage);


  // Met à jour la couleur du NeoPixel
  updateBatteryLED(batteryPercentage);
}


// ==================== COMMUNICATION ESP-NOW ====================

// Fonction appelée automatiquement lorsqu'un message ESP-NOW est reçu
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {

  // Vérifie que le message n'est pas trop grand
  if (len >= sizeof(incomingMessage.command)) {

    Serial.println("Error: Incoming data too large!");
    return;
  }


  // Copie le message reçu
  memcpy(incomingMessage.command, incomingData, len);
  incomingMessage.command[len] = '\0';


  // ==================== LECTURE DU JSON ====================
  
  JsonDocument doc;

  // Transforme le message reçu en données JSON utilisables
  DeserializationError error = deserializeJson(doc, incomingMessage.command);


  // Vérifie si le JSON est valide
  if (error) {

    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }


  // Valeurs X et Y du joystick
  int x = 0, y = 0;
  

  // Parcourt toutes les informations contenues dans le JSON
  for (JsonPair kv : doc.as<JsonObject>()) {

    const char* key = kv.key().c_str();


    // ==================== BOUTONS ====================
    
    // Vérifie si la valeur reçue est un bouton (true ou false)
    if (kv.value().is<bool>()) {


      // Sauvegarde l'ancien et le nouvel état du bouton SELECT
      if (strcmp(key, "Se") == 0) {

        previousSeState = currentSeState;
        currentSeState = kv.value().as<bool>();
      }


      // Traite le bouton lorsque sa valeur est false
      if(!kv.value().as<bool>()) {


        // Bouton SELECT : allume ou éteint les phares
        if (strcmp(key, "Se") == 0) {

          if (!currentSeState && previousSeState) {

            Serial.println("SELECT button pressed, toggling lights");
            toggleLights();
          } 


        // Bouton U : haute puissance des phares
        } else if (strcmp(key, "U") == 0) {

          if (LED_STATE) {

            Serial.println("U button pressed, setting lights to high beam power");

            lights_power = HIGH_BEAM_POWER;
            ledcWrite(lights_pwm_channel, percentToPWM(lights_power));
          }


        // Bouton D : basse puissance des phares
        } else if (strcmp(key, "D") == 0) {

          if (LED_STATE) {

            Serial.println("D button pressed, setting lights to low beam power");

            lights_power = LOW_BEAM_POWER;
            ledcWrite(lights_pwm_channel, percentToPWM(lights_power));
          }
        }
      }
    }


    // ==================== JOYSTICK ====================
    
    // Vérifie si la valeur reçue est un nombre
    else if (kv.value().is<int>()) {

      int analogValue = kv.value().as<int>();


      // Axe X = direction gauche/droite
      if (strcmp(key, "jRX") == 0) {

        x = analogValue;


      // Axe Y = avance/recul
      } else if (strcmp(key, "jRY") == 0) {

        y = analogValue;
      }
    }
  }
  

  // Envoie X et Y à la fonction qui contrôle les moteurs
  rcCar_cmd(x, y);
}


// ==================== CONTRÔLE DES PHARES ====================

void toggleLights() {

  if(LED_STATE) {

    // Éteint les phares
    ledcWrite(lights_pwm_channel, 0);
    LED_STATE = false;

  } else {

    // Allume les phares à la puissance sélectionnée
    ledcWrite(lights_pwm_channel, percentToPWM(lights_power));
    LED_STATE = true;
  }
}


// ==================== CONTRÔLE DES MOTEURS ====================

// Contrôle la direction et la vitesse selon le joystick
void rcCar_cmd(int x, int y) {


  // Arrête les moteurs si une inclinaison est détectée
  if (tiltDetected) {

    Serial.println("Tilt detected - motors disabled for safety");
    rcCar_stop();
    return;
  }


  // Affiche les valeurs du joystick
  Serial.printf("Joystick X: %d, Y: %d\n", x, y);


  // Zone morte : arrête les moteurs lorsque le joystick est près du centre
  if (abs(x) < 10 && abs(y) < 10) {

        Serial.println("X and Y below threshold, stopping motors");
        rcCar_stop();
  }


  // ==================== CALCUL DU VIRAGE ====================

  // Réduit légèrement la vitesse lors des virages
  float turn_factor = 1.0f - pow(abs(x) / 100.0f, 2.0f);

  // Limite le facteur entre 90 % et 100 %
  turn_factor = max(0.9f, min(1.0f, turn_factor));


  // Inverse la direction X lorsque la voiture recule
  int adjusted_x = (y < -10) ? -x : x;
  

  // Le X influence les moteurs à 50 %
  float weight = 0.5f;

  // Calcule la vitesse de chaque moteur avec X et Y
  int duty_cycle_right = y - (adjusted_x * weight);
  int duty_cycle_left = y + (adjusted_x * weight);


  // Applique le facteur de virage
  duty_cycle_right *= turn_factor;
  duty_cycle_left *= turn_factor;


  // Limite les valeurs entre -100 et 100
  duty_cycle_right = max(-100, min(100, duty_cycle_right));
  duty_cycle_left = max(-100, min(100, duty_cycle_left));


  Serial.printf("Duty Cycle Right: %d, Left: %d\n", duty_cycle_right, duty_cycle_left);


  // ==================== MOTEUR DROIT ====================

  if (duty_cycle_right > 0) {

      // Marche avant
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_cycle_right);
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 0);

  } else {

      // Marche arrière
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0);
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, -duty_cycle_right);
  }


  // ==================== MOTEUR GAUCHE ====================

  if (duty_cycle_left > 0) {

      // Marche avant
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, 0);
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, duty_cycle_left);

  } else {

      // Marche arrière
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, -duty_cycle_left);
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, 0);
  }
}