#ifndef ROBOT_H
#define ROBOT_H

class Robot
{
private:
    controleurMoteurs moteurs;
    controleurLumieres lumieres;
    moniteurBatterie batterie;
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
