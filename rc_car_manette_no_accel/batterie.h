#include "Arduino.h"
#include "Commande.h"


class rccar_batterie {
    public:
        float tension;
        int pourcentage;
        int pin;
        int threshold_r = 0;
        int threshold_j = 0;
        float max = 3.3;
        float min = 3.3;
        void update_values(){
            tension = convert_raw();
            // Convert voltage to battery percentage using MIN_VOLTAGE and MAX_VOLTAGE
            pourcentage = (tension - min) / (max - min) * 100.0;
            if (pourcentage > 100) pourcentage = 100;
            if (pourcentage < 0) pourcentage = 0;
            batterie.tension = tension;
            batterie.pourcentage = pourcentage;
            envoie();
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
            int current_threshold = get_threshold();
            return current_threshold
        };
        rccar_batterie(int new_pin, float n_min, float n_max, int threshold1, int threshold2){
            pin = new_pin;
            min = n_min;
            max = n_max;
            threshold_r = threshold1;
            threshold_j = threshold2;
            led= light;
            update_values();
        };
    private:
        int valeur_raw = 2000;
        int resolution = 12;

        float convert_raw() {
            Serial.println("-------------- Reading battery voltage --------------");
            float max_adc = (float)((1 << resolution) - 1);
            valeur_raw = analogRead(pin);
            float voltage = valeur_raw / max_adc * max + min;
            return voltage;
        }   
};

