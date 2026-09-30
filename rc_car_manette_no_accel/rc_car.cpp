/*
  Ce code :
  - Contrôle les moteurs de la voiture
  - Contrôle les phares
  - Vérifie le niveau de batterie
  - Reçoit les commandes ESP-NOW
  - Lit les boutons et le joystick
  - Arrête les moteurs si la voiture est inclinée
*/

// ==================== BIBLIOTHÈQUES ====================

#include "rc_car.h"               // Fonctions de la voiture
#include "Arduino.h"              // Fonctions Arduino
#include "driver/mcpwm.h"         // Contrôle PWM des moteurs
#include "Adafruit_NeoPixel.h"    // Contrôle des NeoPixel
#include <ArduinoJson.h>          // Lecture des messages JSON
#include <Wire.h>                 // Communication I2C
#include "user_define.h"          // Constantes et broches du projet


// ==================== VARIABLES GLOBALES ====================

// Canal PWM utilisé pour les phares
const int lights_pwm_channel = 0;


// Variables venant du fichier principal
extern bool LED_STATE;
extern bool previousSeState;
extern bool currentSeState;
extern bool tiltDetected;
extern Adafruit_NeoPixel pixelsBattery;


// Structure du message ESP-NOW
typedef struct struct_message {

  // Contient la commande JSON
  char command[256];

} struct_message;


// Message reçu
extern struct_message incomingMessage;


// Puissance actuelle des phares
int lights_power = HIGH_BEAM_POWER;


/*
  percentToPWM()
  Convertit un pourcentage de 0 à 100
  en valeur PWM de 0 à 255.
*/
static uint8_t percentToPWM(int percent) {

  // Garde la valeur entre 0 et 100
  percent = constrain(percent, 0, 100);

  // Convertit 0-100 vers 0-255
  return (uint8_t)map(percent, 0, 100, 0, 255);
}


// ==================== CONTRÔLE DES MOTEURS ====================


/*
  rcCar_setup()
  Initialise les moteurs, les phares
  et les NeoPixel.
*/
void rcCar_setup()
{

  // Broche moteur droit avant
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, RIGHT_MOTOR_FWD);

  // Broche moteur droit arrière
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, RIGHT_MOTOR_BWD);


  // Broche moteur gauche avant
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, LEFT_MOTOR_FWD);

  // Broche moteur gauche arrière
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, LEFT_MOTOR_BWD);

  // Configuration du PWM
  mcpwm_config_t pwm_config;
  // Fréquence PWM de 5 kHz
  pwm_config.frequency = 5000;
  // Vitesse initiale à 0 %
  pwm_config.cmpr_a = 0;
  // Vitesse initiale à 0 %
  pwm_config.cmpr_b = 0;
  // Compteur PWM croissant
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  // Mode PWM normal
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;


  // Initialise le moteur droit
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
  // Initialise le moteur gauche
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);

  // Arrête les moteurs au départ
  rcCar_stop();

  // Initialise le PWM des phares
  ledcSetup(lights_pwm_channel, 5000, 8);

  // Relie le PWM à la broche des phares
  ledcAttachPin(LIGHTS_PIN, lights_pwm_channel);

  // Éteint les phares
  ledcWrite(lights_pwm_channel, 0);

  // Initialise les NeoPixel
  pixelsBattery.begin();

  // Règle la luminosité
  pixelsBattery.setBrightness(255);

  // Met les NeoPixel en bleu
  pixelsBattery.fill(0x0000FF);

  // Affiche la couleur
  pixelsBattery.show();
}


/*
  rcCar_stop()
  Arrête immédiatement les deux moteurs.
*/
void rcCar_stop(){

  // Affiche un message
  Serial.println("Stopping motors");

  // Arrête moteur droit avant
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0);

  // Arrête moteur droit arrière
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 0);

  // Arrête moteur gauche avant
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, 0);

  // Arrête moteur gauche arrière
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, 0);
}


// ==================== BATTERIE ====================


/*
  updateBatteryLED()
  Change la couleur du NeoPixel
  selon le niveau de batterie.
*/
void updateBatteryLED(int batteryPercentage) {

  // Batterie faible
  if(batteryPercentage < BATTERY_RED_THRESHOLD) {

    Serial.println("Battery low, setting LED to red");

    // Rouge
    pixelsBattery.fill(0xFF0000);

  // Batterie moyenne
  } else if (batteryPercentage < BATTERY_YELLOW_THRESHOLD) {

    Serial.println("Battery medium, setting LED to yellow");

    // Jaune
    pixelsBattery.fill(0xFFFF00);

  } else {

    // Batterie bonne
    Serial.println("Battery good, setting LED to green");

    // Vert
    pixelsBattery.fill(0x00FF00);
  }

  // Affiche la couleur
  pixelsBattery.show();
}


/*
  getBatteryPercentage()
  Lit la tension de la batterie
  et calcule son pourcentage.
*/
void getBatteryPercentage() {

  // Affiche un message
  Serial.println("-------------- Reading battery voltage --------------");

  // Lit et calcule la tension
  float voltage = analogRead(ADC_BATTERY_PIN) / 4095.0f * 3.3f * 2 + 0.24f;

  // Affiche le texte
  Serial.print("Voltage: ");

  // Affiche la tension
  Serial.println(voltage);


  // Convertit la tension en pourcentage
  int batteryPercentage = map(voltage * 1000, MIN_VOLTAGE * 1000, MAX_VOLTAGE * 1000, 0, 100);

  // Limite à 100 %
  if (batteryPercentage > 100) batteryPercentage = 100;

  // Limite à 0 %
  if (batteryPercentage < 0) batteryPercentage = 0;


  // Affiche le texte
  Serial.print("Battery Percentage: ");

  // Affiche le pourcentage
  Serial.println(batteryPercentage);


  // Met à jour la couleur de batterie
  updateBatteryLED(batteryPercentage);
}


// ==================== COMMUNICATION ESP-NOW ====================
/*
  onDataRecv()
  Reçoit le message ESP-NOW,
  lit le JSON et traite les commandes.
*/
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {

  // Vérifie si le message est trop grand
  if (len >= sizeof(incomingMessage.command)) {

    // Affiche une erreur
    Serial.println("Error: Incoming data too large!");

    // Quitte la fonction
    return;
  }


  // Copie le message reçu
  memcpy(incomingMessage.command, incomingData, len);

  // Ajoute la fin de chaîne
  incomingMessage.command[len] = '\0';


  // ==================== LECTURE JSON ====================
  // Crée le document JSON
  JsonDocument doc;

  // Transforme le texte reçu en JSON
  DeserializationError error = deserializeJson(doc, incomingMessage.command);


  // Vérifie si le JSON contient une erreur
  if (error) {

    // Affiche le texte d'erreur
    Serial.print("deserializeJson() failed: ");

    // Affiche l'erreur
    Serial.println(error.f_str());

    // Quitte la fonction
    return;
  }


  // Valeurs du joystick
  int x = 0, y = 0;


  // Parcourt toutes les valeurs JSON
  for (JsonPair kv : doc.as<JsonObject>()) {

    // Récupère le nom de la commande
    const char* key = kv.key().c_str();


    // ==================== BOUTONS ====================
    // Vérifie si la valeur est un bouton
    if (kv.value().is<bool>()) {

      // Vérifie le bouton SELECT
      if (strcmp(key, "Se") == 0) {

        // Sauvegarde l'ancien état
        previousSeState = currentSeState;

        // Sauvegarde le nouvel état
        currentSeState = kv.value().as<bool>();
      }


      // Vérifie si le bouton est appuyé
      if(!kv.value().as<bool>()) {

        // Bouton SELECT
        if (strcmp(key, "Se") == 0) {

          // Détecte un nouvel appui
          if (!currentSeState && previousSeState) {

            Serial.println("SELECT button pressed, toggling lights");

            // Allume ou éteint les phares
            toggleLights();
          }

        // Bouton U
        } else if (strcmp(key, "U") == 0) {

          // Vérifie si les phares sont allumés
          if (LED_STATE) {

            Serial.println("U button pressed, setting lights to high beam power");

            // Met les phares en mode fort
            lights_power = HIGH_BEAM_POWER;

            // Applique la puissance
            ledcWrite(lights_pwm_channel, percentToPWM(lights_power));
          }

        // Bouton D
        } else if (strcmp(key, "D") == 0) {

          // Vérifie si les phares sont allumés
          if (LED_STATE) {

            Serial.println("D button pressed, setting lights to low beam power");

            // Met les phares en mode faible
            lights_power = LOW_BEAM_POWER;

            // Applique la puissance
            ledcWrite(lights_pwm_channel, percentToPWM(lights_power));
          }
        }

        // D'autres boutons peuvent être ajoutés ici
      }


    // ==================== JOYSTICK ====================
    // Vérifie si la valeur est un nombre
    } else if (kv.value().is<int>()) {

      // Récupère la valeur
      int analogValue = kv.value().as<int>();


      // Axe X du joystick droit
      if (strcmp(key, "jRX") == 0) {

        // Sauvegarde X
        x = analogValue;

      // Axe Y du joystick droit
      } else if (strcmp(key, "jRY") == 0) {

        // Sauvegarde Y
        y = analogValue;
      }

      // D'autres valeurs analogiques peuvent être ajoutées ici
    }
  }


  // Envoie les valeurs du joystick aux moteurs
  rcCar_cmd(x, y);
}


/*
  toggleLights()
  Allume ou éteint les phares.
*/
void toggleLights() {

  // Si les phares sont allumés
  if(LED_STATE) {

    // Éteint les phares
    ledcWrite(lights_pwm_channel, 0);

    // Sauvegarde l'état
    LED_STATE = false;

  } else {

    // Allume les phares
    ledcWrite(lights_pwm_channel, percentToPWM(lights_power));

    // Sauvegarde l'état
    LED_STATE = true;
  }
}


// ==================== CONTRÔLE DES MOTEURS ====================
/*
  rcCar_cmd()
  Utilise le joystick pour contrôler
  la direction et la vitesse des moteurs.
*/
void rcCar_cmd(int x, int y) {


  // ==================== SÉCURITÉ ====================
  // Vérifie si la voiture est trop inclinée
  if (tiltDetected) {

    Serial.println("Tilt detected - motors disabled for safety");

    // Arrête les moteurs
    rcCar_stop();

    // Quitte la fonction
    return;
  }

  // Affiche les valeurs du joystick
  Serial.printf("Joystick X: %d, Y: %d\n", x, y);


  // ==================== ZONE MORTE ====================
  // Arrête les moteurs si le joystick est presque au centre
  if (abs(x) < 10 && abs(y) < 10) {

        Serial.println("X and Y below threshold, stopping motors");

        // Arrête les moteurs
        rcCar_stop();
  }


  // ==================== VIRAGE ====================
  // Calcule un facteur pour adoucir les virages
  float turn_factor = 1.0f - pow(abs(x) / 100.0f, 2.0f);

  // Garde le facteur entre 0.9 et 1.0
  turn_factor = max(0.9f, min(1.0f, turn_factor));


  // ==================== CALCUL DES MOTEURS ====================
  // Inverse la direction X en marche arrière
  int adjusted_x = (y < -10) ? -x : x;

  // Influence du joystick X sur le virage
  float weight = 0.5f;

  // Calcule la vitesse du moteur droit
  int duty_cycle_right = y - (adjusted_x * weight);

  // Calcule la vitesse du moteur gauche
  int duty_cycle_left = y + (adjusted_x * weight);

  // Adoucit le moteur droit dans les virages
  duty_cycle_right *= turn_factor;

  // Adoucit le moteur gauche dans les virages
  duty_cycle_left *= turn_factor;

  // Limite le moteur droit entre -100 et 100
  duty_cycle_right = max(-100, min(100, duty_cycle_right));

  // Limite le moteur gauche entre -100 et 100
  duty_cycle_left = max(-100, min(100, duty_cycle_left));

  // Affiche les vitesses calculées
  Serial.printf("Duty Cycle Right: %d, Left: %d\n", duty_cycle_right, duty_cycle_left);


  // ==================== MOTEUR DROIT ====================
  // Si le moteur droit doit avancer
  if (duty_cycle_right > 0) {
      // Fait avancer le moteur droit
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_cycle_right);

      // Désactive la marche arrière
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 0);

  } else {
      // Désactive la marche avant
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0);

      // Fait reculer le moteur droit
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, -duty_cycle_right);
  }


  // ==================== MOTEUR GAUCHE ====================
  // Si le moteur gauche doit avancer
  if (duty_cycle_left > 0) {
      // Désactive une direction
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, 0);

      // Contrôle le moteur gauche
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, duty_cycle_left);

  } else {
      // Contrôle le moteur gauche dans l'autre direction
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, -duty_cycle_left);

      // Désactive l'autre direction
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, 0);
  }
}