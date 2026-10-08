#include <stdint.h>
#include "Arduino.h"

class lumiere{

};

class batterie {
    public:
        float tension;
        int pourcentage;
        int pin;
        int threshold_r = 0;
        int threshold_j = 0;
        float max = 3.3;
        float min = 3.3;
        lumiere led;
        void update_values(){
            tension = convert_raw();
            // Convert voltage to battery percentage using MIN_VOLTAGE and MAX_VOLTAGE
            pourcentage = map(tension * 1000, min * 1000, max * 1000, 0, 100);
            if (pourcentage > 100) pourcentage = 100;
            if (pourcentage < 0) pourcentage = 0;

            Serial.println(tension);
            Serial.println(pourcentage);
        };
        int get_threshold(){
            if(pourcentage < threshold_r) {
                return 2;
            } else if (pourcentage < threshold_j) {
                return 1;
            } else {
                return 0;
            }

        };
        void updateBatteryLED(){
            current_threshold = get_threshold()
            if (current_threshold == 2){
                Serial.println("Battery low, setting LED to red");
            } else if (current_threshold == 2){
                Serial.println("Battery medium, setting LED to yellow");
            } else {
                Serial.println("Battery good, setting LED to green");
            }
        };
        batterie(int new_pin, float n_min, float n_max, int threshold1, int threshold2){
            pin = new_pin;
            min = n_min;
            max = n_max;
            threshold_r = threshold1;
            threshold_j = threshold2;
            update_values();
        };
    private:
        uint16_t valeur_raw = 2000;
        int resolution = 12;

        float convert_raw() {
            Serial.println("-------------- Reading battery voltage --------------");
            float max_adc = (float)((1 << resolution) - 1);
            valeur_raw = analogRead(pin);
            float voltage = valeur_raw / max_adc * max + min;
            return voltage;
        }   
};
int PIN = A0;

batterie bat(PIN, 0, 3.3 ,2 ,1 );

void setup() {
  Serial.begin(9600);
  Serial.println(PIN);
}
void loop() {
    bat.update_values();
    bat.updateBatteryLED();
    delay((1000));
}