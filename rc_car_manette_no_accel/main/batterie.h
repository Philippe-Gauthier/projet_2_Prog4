#include "Arduino.h"
#include "Information.h"


class rccar_batterie : public Information{
    public:
        float tension;
        int pourcentage;
        int pin = 1;
        int threshold_r = 20;
        int threshold_j = 50;
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
            //Get the threshold. Its either 0,1,2
            if(pourcentage < threshold_r) {
                return 2;
            } else if (pourcentage < threshold_j) {
                return 1;
            } else {
                return 0;
            }

        };

        void init(){
            update_values();
        };
    private:
        int valeur_raw = 2000;
        int resolution = 12;

        float convert_raw() {
            float max_adc = (float)((1 << resolution) - 1);
            valeur_raw = analogRead(pin);
            float voltage = valeur_raw / max_adc * max + min;
            return voltage;
        }   
};
