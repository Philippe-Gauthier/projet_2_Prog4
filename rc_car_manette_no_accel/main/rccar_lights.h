#pragma once // protection pour empêcher les redéclarations et redéfintions multiples qui peuvent potentiellement causer des erreurs
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Commande.h>

class rccar_lights : public Commande {
public:
    int Neopixel_brightness = 255;
    int lights_power = 100;
    bool lights_on = false;
    
    void setupLights() {
        ledcAttach(lights_pin, 5000, 8);

        ledcWrite(lights_pin, 0);
        
        pixelsBattery.begin();
        pixelsBattery.setBrightness(Neopixel_brightness);
        pixelsBattery.fill(0x0000FF);
        pixelsBattery.show();
    }
    
    void updateLED(){
        if(manettes.Se == 0 && Se_last == 1) {
            toggleLights();            
        }
        Se_last = manettes.Se;
    } // Checks for level change 
    
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

        }
        pixelsBattery.show();
    } // Update battery status LED color based on battery percentage
    
    private:
    const int lights_pwm_channel = 0;
    const int Neopixel_battery_pin = 4;
    const int Neopixel_battery_number = 1;
    const int lights_pin = 43;
    bool Se_last = manettes.Se;
    Adafruit_NeoPixel pixelsBattery{Neopixel_battery_number, Neopixel_battery_pin, NEO_GRB + NEO_KHZ800};
    
    int percentToPWM(int percent) {
        return map(percent, 0, 100, 0, 255);
    }
    
    void toggleLights(){
        if(lights_on) {
            ledcWrite(lights_pin, 0);
            lights_on = false;
        } else {
            ledcWrite(lights_pin, percentToPWM(lights_power));
            lights_on = true;
        }
    } // Toggle the lights on/off
};