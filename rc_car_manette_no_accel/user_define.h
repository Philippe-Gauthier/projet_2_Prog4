/*
  Ce fichier contient toutes les constantes du projet :
  - Broches de la caméra
  - Broches de la carte
  - Batterie
  - Phares
  - Moteurs
  - Bouton
  - NeoPixel
  - Mode DEBUG
*/
#ifndef USER_DEFINE_H
#define USER_DEFINE_H

// ==================== CAMÉRA ====================
// Broche Power Down
#define PWDN_GPIO_NUM     -1

// Broche Reset
#define RESET_GPIO_NUM    6

// Horloge de la caméra
#define XCLK_GPIO_NUM     10

// Données I2C de la caméra
#define SIOD_GPIO_NUM     40

// Horloge I2C de la caméra
#define SIOC_GPIO_NUM     39

// Broches de données de la caméra
#define Y9_GPIO_NUM       41
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15

// Synchronisation verticale
#define VSYNC_GPIO_NUM    38

// Synchronisation horizontale
#define HREF_GPIO_NUM     5

// Horloge des pixels
#define PCLK_GPIO_NUM     13

// ==================== BROCHES DE LA CARTE ====================
// Broche du NeoPixel de batterie
#define NEOPIXEL_BATTERY_PIN 4

// Nombre de NeoPixel
#define NEOPIXEL_BATTERY_NUMBER 1

// Broche ADC pour lire la batterie
#define ADC_BATTERY_PIN 1

// Broche des phares
#define LIGHTS_PIN 43

// ==================== BATTERIE ====================
// Tension maximale de la batterie
#define MAX_VOLTAGE 4.2

// Tension minimale de la batterie
#define MIN_VOLTAGE 3.5

// Seuil pour batterie faible
#define BATTERY_RED_THRESHOLD 20

// Seuil pour batterie moyenne
#define BATTERY_YELLOW_THRESHOLD 50

// ==================== PHARES ====================
// Puissance des feux de route
#define HIGH_BEAM_POWER 100

// Puissance des feux de croisement
#define LOW_BEAM_POWER 50

// ==================== MOTEURS ====================
// Moteur droit vers l'avant
#define RIGHT_MOTOR_FWD 2

// Moteur droit vers l'arrière
#define RIGHT_MOTOR_BWD 45

// Moteur gauche vers l'avant
#define LEFT_MOTOR_FWD 44

// Moteur gauche vers l'arrière
#define LEFT_MOTOR_BWD 42

// ==================== BOUTON ====================
// Broche du bouton
#define BUTTON_PIN 0

// ==================== NEOPIXEL ====================
// Luminosité du NeoPixel
#define NEOPIXEL_BRIGHTNESS 100

// ==================== DEBUG ====================
// 1 = DEBUG activé
// 0 = DEBUG désactivé
#define DEBUG 1

#endif // USER_DEFINE_H