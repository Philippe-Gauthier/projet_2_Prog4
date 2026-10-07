// Définition des fonctions utilisées dans le code

#include "rc_car.h"
#include "Arduino.h"
#include "driver/mcpwm.h"
#include "Adafruit_NeoPixel.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include "user_define.h"

// LEDC channel for lights PWM
const int lights_pwm_channel = 0;

// ==================== EXTERNAL GLOBAL VARIABLES ====================
// (Declared in main.cpp)

// Déclarations des variables globales pour les lumières
extern bool LED_STATE;
extern bool previousSeState;
extern bool currentSeState;
extern bool tiltDetected;
extern Adafruit_NeoPixel pixelsBattery;

// Structure pour la réception des commandes
typedef struct struct_message {
  char command[256];
} struct_message;

// Définition d'une variable pour la commande reçue
extern struct_message incomingMessage;

// Définition d'une variable pour l'intensité de la lumière
// Lights power percentage (used for PWM mapping)
int lights_power = HIGH_BEAM_POWER; // default to high beam percentage (0-100)

// Fonction pour gérer la vitesse des moteur de pourcentage à PWM
/**
 * Convert a 0-100 percentage into 8-bit PWM value (0-255)
 * @param percent Input percentage (0-100)
 * @return PWM value (0-255)
 */
static uint8_t percentToPWM(int percent) {
  percent = constrain(percent, 0, 100);
  return (uint8_t)map(percent, 0, 100, 0, 255);
}

// ==================== MOTOR CONTROL ====================

// Fonction pour l'initialization et la configuration des moteurs 
/**
 * Initialize motor control using MCPWM (Motor Control PWM)
 * Configures PWM for dual motor control:
 *   - Right Motor: GPIO pins for forward/backward
 *   - Left Motor: GPIO pins for forward/backward
 * PWM Frequency: 5kHz, Duty range: 0-100%
 */
void rcCar_setup()
{
  // Initialization des pins des moteurs
  // Initialize MCPWM GPIO pins
  // Right motor: Forward (MCPWM0A) and Backward (MCPWM0B)
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, RIGHT_MOTOR_FWD);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, RIGHT_MOTOR_BWD);
  
  // Left motor: Forward (MCPWM1A) and Backward (MCPWM1B)
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, LEFT_MOTOR_FWD);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, LEFT_MOTOR_BWD);

  // Configuration des différents paramètres pour le contrôle des moteurs
  // Configure MCPWM parameters
  mcpwm_config_t pwm_config;
  pwm_config.frequency = 5000;              // 5kHz PWM frequency
  pwm_config.cmpr_a = 0;                    // Initial duty cycle 0%
  pwm_config.cmpr_b = 0;
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

  // Initialization des timers pour les moteurs
  // Initialize MCPWM timers for both motors
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config); // Timer 0 for right motor
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config); // Timer 1 for left motor

  // Appel de la fonction pour arrêter les moteurs
  // Stop motors initially
  rcCar_stop();

  // Initialization de la lumière pour indiquer le PWM
  // Initialize LEDC for lights PWM control
  ledcSetup(lights_pwm_channel, 5000, 8); // 5kHz frequency, 8-bit resolution
  ledcAttachPin(LIGHTS_PIN, lights_pwm_channel);
  // initialize pin to off
  ledcWrite(lights_pwm_channel, 0);

  // Initialization du néopixel
  // Initialize NeoPixel RGB LEDs
  pixelsBattery.begin();
  pixelsBattery.setBrightness(255);  // Set brightness to 30/255
  pixelsBattery.fill(0x0000FF);      // Fill blue to indicate startup
  pixelsBattery.show();
}

// Fonction pour arrêter les moteurs et indiquer l'arrêt des moteurs dans le terminal
/**
 * Stop all motors immediately (emergency stop)
 * Sets all PWM duty cycles to 0%
 */
void rcCar_stop(){
  Serial.println("Stopping motors");
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0); // Stop right forward
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 0); // Stop right backward
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, 0); // Stop left forward
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, 0); // Stop left backward
}

// ==================== BATTERY MONITORING ====================

// Fonction pour changer la couleur de la lumière et envoyer un message dans le terminal selon le pourcentage de la batterie
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
    pixelsBattery.fill(0xFF0000);         // Red = low battery warning
  } else if (batteryPercentage < BATTERY_YELLOW_THRESHOLD) {
    Serial.println("Battery medium, setting LED to yellow");
    pixelsBattery.fill(0xFFFF00);         // Yellow = medium battery
  } else {
    Serial.println("Battery good, setting LED to green");
    pixelsBattery.fill(0x00FF00);         // Green = good battery
  }
  pixelsBattery.show();
}

// Fonction pour obtenir le pourcentage de la batterie et pour indiquer le pourcentage dans le terminal
/**
 * Read battery voltage via ADC
 */
void getBatteryPercentage() {
  // Read ADC value and convert to voltage
  // ADC formula: voltage = (ADC / 4095) * 3.3V * 2 + calibration offset
  Serial.println("-------------- Reading battery voltage --------------");
  // Lecture de la valeur mesurée et transformation en voltage
  float voltage = analogRead(ADC_BATTERY_PIN) / 4095.0f * 3.3f * 2 + 0.24f;
  Serial.print("Voltage: ");
  Serial.println(voltage);
  
  // Convertir le voltage de la batterue en pourcentage
  // Convert voltage to battery percentage using MIN_VOLTAGE and MAX_VOLTAGE
  int batteryPercentage = map(voltage * 1000, MIN_VOLTAGE * 1000, MAX_VOLTAGE * 1000, 0, 100);
  if (batteryPercentage > 100) batteryPercentage = 100;
  if (batteryPercentage < 0) batteryPercentage = 0;

  Serial.print("Battery Percentage: ");
  Serial.println(batteryPercentage);

  // Changer la couleur de la lumière selon le pourcentage
  // Update NeoPixel LED color based on battery level
  updateBatteryLED(batteryPercentage);
}

// ==================== ESP-NOW COMMUNICATION ====================

// Fonction pour la réception de données du ESP
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
  // Validate incoming data length
  if (len >= sizeof(incomingMessage.command)) {
    Serial.println("Error: Incoming data too large!");
    return;
  }

  // Copy data to message buffer
  memcpy(incomingMessage.command, incomingData, len);
  incomingMessage.command[len] = '\0'; // Null-terminate string

  // ==================== JSON PARSING ====================
  
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, incomingMessage.command);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }

  // Lecture des valeurs du joystick et du boutton
  int x = 0, y = 0; // Joystick values
  
  // Iterate through JSON key-value pairs
  for (JsonPair kv : doc.as<JsonObject>()) {
    const char* key = kv.key().c_str();

    // ==================== BUTTON HANDLING ====================
    
    if (kv.value().is<bool>()) {
      // Track SELECT button state for edge detection
      if (strcmp(key, "Se") == 0) {
        previousSeState = currentSeState;
        currentSeState = kv.value().as<bool>();
      }

      // Process button release events (button is false when not pressed)
      if(!kv.value().as<bool>()) {
        if (strcmp(key, "Se") == 0) {
          // SELECT button: toggle LED on falling edge
          if (!currentSeState && previousSeState) {
            Serial.println("SELECT button pressed, toggling lights");
            toggleLights();
          } 
        } else if (strcmp(key, "U") == 0) {
          // U button: set brightness to 255 on falling edge
          if (LED_STATE) {
            Serial.println("U button pressed, setting lights to high beam power");
            lights_power = HIGH_BEAM_POWER;
            ledcWrite(lights_pwm_channel, percentToPWM(lights_power));
          }
        } else if (strcmp(key, "D") == 0) {
          // D button: set brightness to 128 on falling edge
          if (LED_STATE) {
            Serial.println("D button pressed, setting lights to low beam power");
            lights_power = LOW_BEAM_POWER;
            ledcWrite(lights_pwm_channel, percentToPWM(lights_power));
          }
        }
        // Other button press events can be added here if needed
      }

    // ==================== JOYSTICK/ANALOG HANDLING ====================
    
    } else if (kv.value().is<int>()) {
      int analogValue = kv.value().as<int>();

      // Right joystick controls motor movement (X=steering, Y=throttle)
      if (strcmp(key, "jRX") == 0) {
        x = analogValue;
      } else if (strcmp(key, "jRY") == 0) {
        y = analogValue;
      }
      // Left joystick and triggers can be added here if needed
    }
  }
  
  // Envoyer les valeurs du joystick à la fonction de controle des moteurs
  // Send joystick values to motor control function
  rcCar_cmd(x, y);
}

// Fonction pour allumer ou fermer les lumières
/**
 * Toggle headlight LED on/off
 * Switches between current brightness level and off
 */
void toggleLights() {
  if(LED_STATE) {
    ledcWrite(lights_pwm_channel, 0);
    LED_STATE = false;
  } else {
    ledcWrite(lights_pwm_channel, percentToPWM(lights_power));
    LED_STATE = true;
  }
}

// ==================== MOTOR COMMAND & CONTROL LOGIC ====================

// Fonction pour contrôler les moteus
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

  // ==================== SAFETY CHECK: TILT DETECTION ====================
  // Stop motors immediately if tilt is detected
  if (tiltDetected) {
    Serial.println("Tilt detected - motors disabled for safety");
    rcCar_stop();
    return;
  }

  // Debug: Print joystick values
  Serial.printf("Joystick X: %d, Y: %d\n", x, y);

  // ==================== DEAD ZONE & MOTOR STOP ====================
  // Stop motors if joystick is below threshold (accounts for joystick jitter)
  if (abs(x) < 10 && abs(y) < 10) {
        Serial.println("X and Y below threshold, stopping motors");
        rcCar_stop();
    }

  // ==================== TURN FACTOR (SMOOTH TURNING) ====================
  // Reduce motor speed during turns to maintain traction
  // Formula: turn_factor = 1.0 - (normalized_x)^2
  // This creates non-linear scaling where small steering inputs have minimal speed reduction
  float turn_factor = 1.0f - pow(abs(x) / 100.0f, 2.0f); // Use a smoother non-linear scaling
  turn_factor = max(0.9f, min(1.0f, turn_factor)); // Clamp turn_factor to a minimum of 90% and a maximum of 100%

  // ==================== STEERING & THROTTLE CALCULATION ====================
  // Adjust steering direction based on forward/backward movement
  int adjusted_x = (y < -10) ? -x : x; // Adjust x for backward movement
  
  // Apply weighted steering to throttle values
  // weight = 0.5 means 50% of steering value is mixed with throttle
  float weight = 0.5f; // Reduce the influence of x on turning
  int duty_cycle_right = y - (adjusted_x * weight); // Combine forward/backward (y) and turning (x) for the right motor
  int duty_cycle_left = y + (adjusted_x * weight);  // Combine forward/backward (y) and turning (x) for the left motor

  // Apply turn_factor to smooth the turning effect
  duty_cycle_right *= turn_factor;
  duty_cycle_left *= turn_factor;

  // Clamp duty cycles to the range -100 to 100
  duty_cycle_right = max(-100, min(100, duty_cycle_right));
  duty_cycle_left = max(-100, min(100, duty_cycle_left));

  Serial.printf("Duty Cycle Right: %d, Left: %d\n", duty_cycle_right, duty_cycle_left);

  // ==================== DIRECTION & SPEED CONTROL ====================
  // Set motor directions and duty cycles
  if (duty_cycle_right > 0) {
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_cycle_right); // Right motor forward
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 0);                // Stop right motor backward
  } else {
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0);                // Stop right motor forward
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, -duty_cycle_right); // Right motor backward
  }

  if (duty_cycle_left > 0) {
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, 0);  // Left motor forward
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, duty_cycle_left); // Stop left motor backward
  } else {
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, -duty_cycle_left); // Stop left motor forward
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, 0); // Left motor backward
  }
}
