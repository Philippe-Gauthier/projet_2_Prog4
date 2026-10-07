#include "rc_car.h"
#include "Arduino.h"
#include "driver/mcpwm.h"
#include "Adafruit_NeoPixel.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include "user_define.h"


// ==================== CONFIGURATION DES LUMIÈRES ====================

// Canal PWM utilisé pour contrôler la puissance des phares
const int lights_pwm_channel = 0;


// ==================== VARIABLES GLOBALES EXTERNES ====================

// Ces variables sont créées dans main.cpp et utilisées ici
extern bool LED_STATE;
extern bool previousSeState;
extern bool currentSeState;
extern bool tiltDetected;
extern Adafruit_NeoPixel pixelsBattery;


// Structure utilisée pour recevoir la commande JSON par ESP-NOW
typedef struct struct_message {
  char command[256];
} struct_message;

extern struct_message incomingMessage;

// Lights power percentage (used for PWM mapping)
int lights_power = HIGH_BEAM_POWER; // default to high beam percentage (0-100)

/**
 * Convert a 0-100 percentage into 8-bit PWM value (0-255)
 * @param percent Input percentage (0-100)
 * @return PWM value (0-255)
 */
static uint8_t percentToPWM(int percent) {
  percent = constrain(percent, 0, 100);
  return (uint8_t)map(percent, 0, 100, 0, 255);
}


// ==================== CONTRÔLE DES MOTEURS ====================

// Initialise les moteurs, leur PWM, les phares et le NeoPixel
void rcCar_setup()
{
  // Configuration des broches du moteur droit
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, RIGHT_MOTOR_FWD);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, RIGHT_MOTOR_BWD);

  // Configuration des broches du moteur gauche
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, LEFT_MOTOR_FWD);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, LEFT_MOTOR_BWD);

  // Configure MCPWM parameters
  mcpwm_config_t pwm_config;
  pwm_config.frequency = 5000;              // Fréquence PWM de 5 kHz
  pwm_config.cmpr_a = 0;                    // Moteurs arrêtés au départ
  pwm_config.cmpr_b = 0;
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;


  // Timer 0 pour le moteur droit et Timer 1 pour le moteur gauche
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);


  // S'assure que les moteurs sont arrêtés au démarrage
  rcCar_stop();


  // Configuration du PWM des phares : 5 kHz et résolution de 8 bits
  ledcSetup(lights_pwm_channel, 5000, 8);
  ledcAttachPin(LIGHTS_PIN, lights_pwm_channel);
  ledcWrite(lights_pwm_channel, 0); // Phares éteints au départ


  // Initialisation du NeoPixel de la batterie
  pixelsBattery.begin();
  pixelsBattery.setBrightness(255);
  pixelsBattery.fill(0x0000FF); // Bleu au démarrage
  pixelsBattery.show();
}


// Arrête immédiatement les deux moteurs
void rcCar_stop(){
  Serial.println("Stopping motors");

  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 0);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, 0);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, 0);
}


/**
 * Update battery status LED color based on battery percentage
 * Color thresholds:
 *   - Red: <20%
 *   - Yellow: 20-50%
 *   - Green: >50%
 */
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

/**
 * Read battery voltage via ADC
 */
void getBatteryPercentage() {

  Serial.println("-------------- Reading battery voltage --------------");

  // Conversion de la lecture ADC en tension réelle de batterie
  float voltage = analogRead(ADC_BATTERY_PIN) / 4095.0f * 3.3f * 2 + 0.24f;

  Serial.print("Voltage: ");
  Serial.println(voltage);


  // Transforme la tension entre MIN_VOLTAGE et MAX_VOLTAGE en 0 à 100 %
  int batteryPercentage = map(voltage * 1000, MIN_VOLTAGE * 1000, MAX_VOLTAGE * 1000, 0, 100);

  // Empêche le pourcentage de dépasser 0 à 100 %
  if (batteryPercentage > 100) batteryPercentage = 100;
  if (batteryPercentage < 0) batteryPercentage = 0;


  Serial.print("Battery Percentage: ");
  Serial.println(batteryPercentage);


  // Met à jour la couleur du NeoPixel
  updateBatteryLED(batteryPercentage);
}


/**
 * ESP-NOW receive callback - processes incoming JSON commands from remote controller
 * Parses JSON message containing:
 *   - Joystick X/Y values (0-127, center ~64)
 *   - Button states (digital on/off)
 *   - Trigger values (analog 0-255)
 * 
 * @param mac Sender's MAC address (not used in this implementation)
 * @param incomingData Pointer to received data buffer
 * @param len Length of received data
 */
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {

  // Vérifie que le message n'est pas trop gros pour le tableau
  if (len >= sizeof(incomingMessage.command)) {
    Serial.println("Error: Incoming data too large!");
    return;
  }


  // Copie les données reçues dans notre structure
  memcpy(incomingMessage.command, incomingData, len);
  incomingMessage.command[len] = '\0'; // Ajoute la fin de chaîne


  // ==================== LECTURE DU JSON ====================

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, incomingMessage.command);

  // Arrête le traitement si le JSON est invalide
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }

  int x = 0, y = 0; // Joystick values
  
  // Iterate through JSON key-value pairs
  for (JsonPair kv : doc.as<JsonObject>()) {

    const char* key = kv.key().c_str();


    // ==================== GESTION DES BOUTONS ====================

    if (kv.value().is<bool>()) {

      // Sauvegarde l'ancien et le nouvel état du bouton SELECT
      if (strcmp(key, "Se") == 0) {
        previousSeState = currentSeState;
        currentSeState = kv.value().as<bool>();
      }


      // Traite les boutons lorsqu'ils deviennent false
      if(!kv.value().as<bool>()) {

        if (strcmp(key, "Se") == 0) {

          // SELECT permet d'allumer ou d'éteindre les phares
          if (!currentSeState && previousSeState) {
            Serial.println("SELECT button pressed, toggling lights");
            toggleLights();
          }

        } else if (strcmp(key, "U") == 0) {

          // Bouton U : phares à haute puissance
          if (LED_STATE) {
            Serial.println("U button pressed, setting lights to high beam power");
            lights_power = HIGH_BEAM_POWER;
            ledcWrite(lights_pwm_channel, percentToPWM(lights_power));
          }

        } else if (strcmp(key, "D") == 0) {

          // Bouton D : phares à basse puissance
          if (LED_STATE) {
            Serial.println("D button pressed, setting lights to low beam power");
            lights_power = LOW_BEAM_POWER;
            ledcWrite(lights_pwm_channel, percentToPWM(lights_power));
          }
        }
      }


    // ==================== GESTION DU JOYSTICK ====================

    } else if (kv.value().is<int>()) {

      int analogValue = kv.value().as<int>();

      // jRX = direction gauche/droite
      if (strcmp(key, "jRX") == 0) {
        x = analogValue;

      // jRY = avance/recul
      } else if (strcmp(key, "jRY") == 0) {
        y = analogValue;
      }
    }
  }
  
  // Send joystick values to motor control function
  rcCar_cmd(x, y);
}

/**
 * Toggle headlight LED on/off
 * Switches between current brightness level and off
 */
void toggleLights() {

  if(LED_STATE) {

    // Éteint les phares
    ledcWrite(lights_pwm_channel, 0);
    LED_STATE = false;

  } else {

    // Allume les phares à la puissance actuellement sélectionnée
    ledcWrite(lights_pwm_channel, percentToPWM(lights_power));
    LED_STATE = true;
  }
}


/**
 * Motor control based on joystick input
 * Implements:
 *   - Dual motor differential steering (tank style)
 *   - Safety check: motors disabled when device is tilted (implemented)
 *   - Dead zone: ignores small joystick movements (<10)
 *   - Turn factor: reduces speed during sharp turns
 *   - Duty cycle mapping: joystick (-100 to 100) → PWM (0-100%)
 * 
 * @param x Right joystick X value (steering): -127 (left) to +127 (right)
 * @param y Right joystick Y value (throttle): -127 (backward) to +127 (forward)
 */
void rcCar_cmd(int x, int y) {


  // ==================== SÉCURITÉ D'INCLINAISON ====================

  // Arrête immédiatement les moteurs si une inclinaison est détectée
  if (tiltDetected) {
    Serial.println("Tilt detected - motors disabled for safety");
    rcCar_stop();
    return;
  }


  // Affiche les valeurs reçues du joystick
  Serial.printf("Joystick X: %d, Y: %d\n", x, y);


  // ==================== ZONE MORTE DU JOYSTICK ====================

  // Arrête les moteurs si le joystick est près de sa position centrale
  if (abs(x) < 10 && abs(y) < 10) {
    Serial.println("X and Y below threshold, stopping motors");
    rcCar_stop();
  }


  // ==================== FACTEUR DE VIRAGE ====================

  // Réduit légèrement la vitesse lors d'un virage
  float turn_factor = 1.0f - pow(abs(x) / 100.0f, 2.0f);

  // Limite le facteur entre 90 % et 100 %
  turn_factor = max(0.9f, min(1.0f, turn_factor));


  // ==================== CALCUL DE LA DIRECTION ====================

  // Inverse la direction du volant lorsque la voiture recule
  int adjusted_x = (y < -10) ? -x : x;


  // Influence de la direction sur la vitesse des moteurs
  float weight = 0.5f;

  // Mélange l'accélération Y et la direction X pour chaque moteur
  int duty_cycle_right = y - (adjusted_x * weight);
  int duty_cycle_left = y + (adjusted_x * weight);


  // Applique le facteur de virage
  duty_cycle_right *= turn_factor;
  duty_cycle_left *= turn_factor;


  // Limite les commandes des moteurs entre -100 % et +100 %
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