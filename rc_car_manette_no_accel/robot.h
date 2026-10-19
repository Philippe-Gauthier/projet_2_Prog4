#ifndef ROBOT_H
#define ROBOT_H

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
};

#endif
