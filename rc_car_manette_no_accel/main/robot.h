#ifndef ROBOT_H
#define ROBOT_H

#include "batterie.h"
#include "Accelerometre.h"
#include "Moteur.h"
#include "rccar_lights.h"
#include "Commande.h"

class Robot
{
private:
    Moteur moteurs;
    rccar_lights lumieres;
    rccar_batterie batterie;
    Accelerometre accelerometre;

    bool robotActif = true;

    void gererMoteurs();
    void gererLumieres();
    void gererBatterie();
    void gererAccelerometre();

public:
    void initialisation();
    void update();
    void stop();
    void reception_commande(const uint8_t *incomingData, int len) {
    lumiere.update(incomingData, len);
    moteur.update(incomingData, len);
  }
};

#endif
