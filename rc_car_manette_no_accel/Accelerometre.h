#include "Arduino.h"

class Accelerometre : public Information{
    Private:
        std::list<float> Donnée_Brute = [1.0, 2.0, 3.0]
        int Threshold = 50
        struct {
        float Accel_X;
        float Accel_Y;
        float Accel_Z;
        } Acceleration_data;
        String Direction = " " // juste pour afficher dans le terminal, aucun effet sur les moteurs

    Private:

        void Accel_init() {
        // demarrage ligne I2C et mise au point des registres au besoin
            print("Accelerometre initialisé");
        }

        list<float> Get_values() { // lecture des registres
            Value_list = [1.0, 2.0, 3.0] // juste pour l'exemple, normalement on va lire les registres;
            //read all the registers;
            //append into list;
            return Value_list;
            }

        // Modifier pour prendre conpte de la nouvelle structure
        void update_values(Get_values, ) { // Met les attributs à jour
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

            print("Acceleration X: " + "Acceleration_data.Accel_X" + " Acceleration Y: " + "Acceleration_data.Accel_Y" + " Acceleration Z: " + "Acceleration_data.Accel_Z" + " Direction: " + "Direction");
            }
        }

        Public:

        void Set_Threshold(int tilt) {
            this->Threshold = tilt;
            print("Threshold set to: " + String(Threshold));
            }

        bool Error_check(int thresh, std::list<int> data) {
            /*if (data[x] > thresh):
            return True
            else:
            return False*/
            return True; // juste pour l'exemple, normalement on va faire la vérification
            }
        }

