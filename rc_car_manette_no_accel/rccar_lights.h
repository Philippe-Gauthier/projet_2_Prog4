#pragma once // protection pour empêcher les redéclarations et redéfintions multiples qui peuvent potentiellement causer des erreurs
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define BATTERY_RED_THRESHOLD    20
#define BATTERY_YELLOW_THRESHOLD 50

class rccar_lights {
public:
    bool LED_STATE = false;
    int Neopixel_brightness = 255;
    int lights_power = 100;

    void setupLights() {
        ledcSetup(lights_pwm_channel, 5000, 8);
        ledcAttachPin(lights_pin, lights_pwm_channel);
        ledcWrite(lights_pwm_channel, 0);

        pixelsBattery.begin();
        pixelsBattery.setBrightness(Neopixel_brightness);
        pixelsBattery.fill(0x0000FF);
        pixelsBattery.show();
    }

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

    void updateBatteryLED(int batteryLED_level) {
        if(batteryLED_level == 2) {
            Serial.println("Battery low, setting LED to red");
            pixelsBattery.fill(0xFF0000);         // Red = low battery warning
        } else if (batteryLED_level == 1) {
            Serial.println("Battery medium, setting LED to yellow");
            pixelsBattery.fill(0xFFFF00);         // Yellow = medium battery
        } else if (batteryLED_level == 0) {
            Serial.println("Battery good, setting LED to green");
            pixelsBattery.fill(0x00FF00);         // Green = good battery
        } else {
            Serial.println("Battery state unknown, setting LED to blue");
            pixelsBattery.fill(0x0000FF);         // Blue = unknown/error
            battery_state = 0;
        }
        pixelsBattery.show();
    } // Update battery status LED color based on battery percentage

private:
    const int lights_pwm_channel = 0;
    const int Neopixel_battery_pin = 4;
    const int Neopixel_battery_number = 1;
    const int lights_pin = 43;
    Adafruit_NeoPixel pixelsBattery{Neopixel_battery_number, Neopixel_battery_pin, NEO_GRB + NEO_KHZ800};

    int percentToPWM(int percent) {
        return map(percent, 0, 100, 0, 255);
    }
};