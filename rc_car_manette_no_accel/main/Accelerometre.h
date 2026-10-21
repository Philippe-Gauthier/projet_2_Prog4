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

        void Accel_init() {
        // demarrage ligne I2C et mise au point des registres au besoin
            Serial.print("Accelerometre initialisé");
        };

        float* Get_values() { // lecture des registres
            float value_list[3] = {};
            //read all the registers;
            //append into list;
            return value_list;
            };

        // Modifier pour prendre conpte de la nouvelle structure
        void update_values() { // Met les attributs à jour
            float* Raw_values = Get_values();
            /*this->Données_Brute = list;
            New_Acceleration_X = calcul pour faire la conversion
            New_Acceleration_Y = calcul pour faire la conversion
            New_Acceleration_Z = calcul pour faire la conversion
            this->Acceleration_Data.Accel_X = New_Acceleration_X;
            this->Acceleration_Data.Accel_Y = New_Acceleration_Y;
            this->Acceleration_Data.Accel_Z = New_Acceleration_Z;
            if (Acceleration_data.Accel_X > 0):
            New_direction = "Avance"
            else if (New_Acceleration < 0):
            New_direction = "Recule"
            this->Direction = New_direction;*/

            Serial.print("Acceleration X: " + String(Acceleration_data.Accel_X) + " Acceleration Y: " + String(Acceleration_data.Accel_Y) + " Acceleration Z: " + String(Acceleration_data.Accel_Z));
            };

    public:

        void Set_Threshold(float tilt) {
            this->Threshold = tilt;
            Serial.print("Threshold set to: " + String(Threshold));
            };

        bool Error_check(float thresh, Acceleration_data) {
            //update_values(Get_values()); // remet les valeurs à jour pour la prochaine vérification
            if (Acceleration_data.Accel_X > thresh){
                // Herité de son parent :information, on peut donc utiliser les méthodes de la classe Information
                accelerometre.state = true;
                envoie();
            }
            else if (Acceleration_data.Accel_Y > thresh){
                accelerometre.state = true;
                envoie();
            }
            else if (Acceleration_data.Accel_Z > thresh){
                accelerometre.state = true;
                envoie();
            }
            else{
                accelerometre.state = false;
                envoie();
            };
            return true; // juste pour l'exemple, normalement on va faire la vérification
        };
    };


