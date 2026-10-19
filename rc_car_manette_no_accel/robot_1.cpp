
#include "Robot.h"
#include "batterie.h"
#include "Accelerometre.h"
#include "Moteur.h"
#include "rccar_lights.h"


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
    gererLumieres();
    gererMoteurs();
    gererBatterie();
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

    moteurs.rcCar_cmd(x, y);
}


void Robot::gererLumieres()
{
    lumieres.updateBatteryLED();
    lumieres.toggleLights();
}


void Robot::gererBatterie()
{
    batterie.update_values();

}


void Robot::gererAccelerometre()
{
    accelerometre.update_values();
}
