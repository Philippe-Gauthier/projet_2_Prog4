#ifndef USER_DEFINE_H
#define USER_DEFINE_H

// Empêche ce fichier d'être inclus plusieurs fois


// ==================== BROCHES DE LA CAMÉRA ====================

#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    6
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39

#define Y9_GPIO_NUM       41
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     5
#define PCLK_GPIO_NUM     13

// Board IOs
#define NEOPIXEL_BATTERY_PIN 4
#define NEOPIXEL_BATTERY_NUMBER 1
#define ADC_BATTERY_PIN 1
#define LIGHTS_PIN 43
#define MAX_VOLTAGE 4.2  // Maximum expected battery voltage (adjust according to your battery)
#define MIN_VOLTAGE 3.5  // Minimum acceptable battery voltage (adjust according to your battery)

// Battery percentage thresholds for neopixel color changes
#define BATTERY_RED_THRESHOLD 20
#define BATTERY_YELLOW_THRESHOLD 50

// Headlight power levels
#define HIGH_BEAM_POWER 100
#define LOW_BEAM_POWER 50

// Motors pins
#define RIGHT_MOTOR_FWD 2
#define RIGHT_MOTOR_BWD 45

// Moteur gauche
#define LEFT_MOTOR_FWD 44
#define LEFT_MOTOR_BWD 42

// button pin
#define BUTTON_PIN 0

// Neopixel brightness levels
#define NEOPIXEL_BRIGHTNESS 100 // Adjust brightness (0-255)

// ==================== NEOPIXEL ====================

// Luminosité du NeoPixel (0 à 255)
#define NEOPIXEL_BRIGHTNESS 100


// ==================== MODE DEBUG ====================

// 1 = debug activé, 0 = debug désactivé
#define DEBUG 1


#endif // USER_DEFINE_H