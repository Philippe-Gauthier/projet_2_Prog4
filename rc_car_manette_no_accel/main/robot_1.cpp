#include robot.h

void Robot::initialisation()
{
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

    moteurs.rcCar_cmd(Commande.decoding_JSON());
    
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
