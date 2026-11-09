#include "Arduino.h"
#include "Information.h"

struct BMI_data
{
float Accel_X;
float Accel_Y;
float Accel_Z;
};

class Accelerometre : public Information{
    private:

        float Donnee_Brute[3] = {0.0, 0.0, 0.0}; // juste pour l'exemple, normalement on va lire les registres;
        float Threshold = 50.0;
        BMI_data Acceleration_data = {0.0, 0.0, 0.0};

        BMI_data Get_values() {
            BMI_data values = {0.0, 0.0, 0.0};

            // Lecture des registres de l'accéléromètre
            // values.Accel_X = ...
            // values.Accel_Y = ...
            // values.Accel_Z = ...

            return values;
        }
        
        void update_values() {
            Acceleration_data = Get_values();

            Serial.print("Acceleration X: ");
            Serial.print(Acceleration_data.Accel_X);

            Serial.print(" Acceleration Y: ");
            Serial.print(Acceleration_data.Accel_Y);

            Serial.print(" Acceleration Z: ");
            Serial.println(Acceleration_data.Accel_Z);
        }

        

    public:

        void Accel_init() {
        // demarrage ligne I2C et mise au point des registres au besoin
            Serial.print("Accelerometre initialisé");
        };

        void Set_Threshold(float tilt) {
            this->Threshold = tilt;
            Serial.print("Threshold set to: " + String(Threshold));
            };

        bool Error_check() {

            //update_values();
            Serial.print("Fonction error_check OK");
            if (
                abs(Acceleration_data.Accel_X) > Threshold ||
                abs(Acceleration_data.Accel_Y) > Threshold ||
                abs(Acceleration_data.Accel_Z) > Threshold
            ) {
                accelerometre.state = true;
                return true;
            }
            else {
                accelerometre.state = false;
                return false;
            }

            envoie();
        }
    };


