#include <stdint.h>


class lumiere{

};

class batterie {
    public:
        float tension;
        int pourcentage;
        int pin;
        int threshold_r = 0;
        int threshold_j = 0;
        int max = 3.3;
        int min = 3.3;
        lumiere led;
        void update_values(){

        };
        int get_threshold(){

        };
        void updateBatteryLED(){

        };
        batterie(int new_pin, int n_min, int n_max, int threshold1, int threshold2){
            min = n_min;
            max = n_max;
            threshold_r = threshold1;
            threshold_j = threshold2;
            update_values();
        };
    private:
        uint16_t valeur_raw;
        int resolution; 
        float convert_raw(){

        };
};