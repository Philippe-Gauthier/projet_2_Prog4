#include "robot.h"

void Robot::initialisation(void (*onDataRecv)(const esp_now_recv_info_t *, const uint8_t *, int ))
{
    accelerometre.initialization_esp(onDataRecv);
    moteurs.rcCar_setup();
    lumieres.setupLights();
    batterie.init();
    accelerometre.Accel_init();

    robotActif = true;
}


void Robot::update()
{
    gererAccelerometre();
    gererBatterie();
    gererLumieres();
    gererMoteurs();
}


void Robot::stop()
{
    moteurs.rcCar_stop();
    robotActif = false;
}


void Robot::gererMoteurs()
{
    if (!robotActif)
    {
        moteurs.rcCar_stop();
        return;
    }

    moteurs.rcCar_cmd();
    
}


void Robot::gererLumieres()
{
    lumieres.updateBatteryLED(batterie.get_threshold());
    lumieres.updateLED();
}


void Robot::gererBatterie()
{
    batterie.update_values();
}


void Robot::gererAccelerometre()
{
    if(accelerometre.Error_check())
    {
        tiltDetected = true;
        robotActif = false;
        return;
    }
}


void Robot::reception_commande(const uint8_t *incomingData, int len)
{
    lumieres.reception_Message(incomingData, len);
    moteurs.reception_Message(incomingData, len);
}
