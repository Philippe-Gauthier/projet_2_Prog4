#include "Adafruit_NeoPixel.h"    // RGB LED strip control library

class rccar_lights{
    public:

        // Variables
        bool LED_STATE = false; // Track the current state of the lights (on/off)
        int Neopixel_brightness; // Brightness level for NeoPixel LEDs (0-255)
        ​​int lights_power; // 0 à 100
        ​int battery_state; // 0 à 3, int indicant le niveau de la batterie: (0: inconnue/erreur, 1: rouge, 2: jaune, 3: verte))
        
        // Functions​
        void toggleLights(){
            if(LED_STATE) {
                ledcWrite(lights_pwm_channel, 0);
                LED_STATE = false;
            } else {
                ledcWrite(lights_pwm_channel, percentToPWM(lights_power));
                LED_STATE = true;
            }
        } // Toggle the lights on/off

        void write2LED(int pourcentage){
            lights_power = pourcentage;
            if(LED_STATE) {
                ledcWrite(lights_pwm_channel, percentToPWM(lights_power));
            }
        } // Write a specific brightness level to the lights (0-100%)

    private:

        // Variables
        ​const int lights_pwm_channel = 0; // utilisé par write2LED
        const ​​int Neopixel_battery_pin = 4;
        const ​int Neopixel_battery_number = 1;
        const int BATTERY_RED_THRESHOLD = 20;
        const int BATTERY_YELLOW_THRESHOLD = 50;
        const int lights_pin = 43; // Pin for controlling the lights (PWM output)
        ​Adafruit_NeoPixel pixelsBattery(Neopixel_battery_number, Neopixel_battery_pin, NEO_GRB + NEO_KHZ800);

        // Functions
        void setupLights() {
            // Initialize LEDC for lights PWM control
            ledcSetup(lights_pwm_channel, 5000, 8); // 5kHz frequency, 8-bit resolution
            ledcAttachPin(lights_pin, lights_pwm_channel);
            // Initialize pin to off
            ledcWrite(lights_pwm_channel, 0);

            // Initialize NeoPixel RGB LEDs
            pixelsBattery.begin();
            pixelsBattery.setBrightness(255);  // Set brightness to maximum
            pixelsBattery.fill(0x0000FF);      // Fill blue to indicate startup
            pixelsBattery.show();
        } // Setup the lights and NeoPixel LEDs

        void percentToPWM(int percent) {
            // Convert percentage (0-100) to PWM value (0-255)
            return map(percent, 0, 100, 0, 255);
        } // Convert percentage to PWM value

        void updateBatteryLED(int batteryPercentage) {
            if(batteryPercentage < BATTERY_RED_THRESHOLD) {
                Serial.println("Battery low, setting LED to red");
                pixelsBattery.fill(0xFF0000);         // Red = low battery warning
                battery_state = 1;
            } else if (batteryPercentage < BATTERY_YELLOW_THRESHOLD) {
                Serial.println("Battery medium, setting LED to yellow");
                pixelsBattery.fill(0xFFFF00);         // Yellow = medium battery
                battery_state = 2;
            } else if (batteryPercentage >= BATTERY_YELLOW_THRESHOLD) {
                Serial.println("Battery good, setting LED to green");
                pixelsBattery.fill(0x00FF00);         // Green = good battery
                battery_state = 3;
            } else {
                Serial.println("Battery state unknown, setting LED to blue");
                pixelsBattery.fill(0x0000FF);         // Blue = unknown/error
                battery_state = 0;
            }
            pixelsBattery.show();
            //return battery_state;
        } // Update battery status LED color based on battery percentage
}