#ifndef MOTEUR_H
#define MOTEUR_H

#include "Commande.h"
#include <Arduino.h>
#include "user_define.h"
#include "driver/mcpwm.h"

// Classe qui controle les deux moteurs de la voiture.
// Elle herite de la classe Commande.
class Moteur : public Commande {

private:
    // Puissance et direction des moteurs (-100 a 100).
    int duty_cycle_right = 0;
    int duty_cycle_left = 0;

    // Variables pour calculer les virages.
    float turn_factor = 1.0;
    int adjusted_x = 0;
    float weight = 0.5;


    // Indique si la voiture est renversee.
    bool tiltDetected = false;

public:

    // Initialise les moteurs et le PWM.
    void rcCar_setup() {

        // Associer les broches aux moteurs.
        mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, RIGHT_MOTOR_FWD);
        mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, RIGHT_MOTOR_BWD);
        mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, LEFT_MOTOR_FWD);
        mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, LEFT_MOTOR_BWD);

        // Configurer le PWM a 5000 Hz.
        mcpwm_config_t pwm = {};
        pwm.frequency = 5000;
        pwm.counter_mode = MCPWM_UP_COUNTER;
        pwm.duty_mode = MCPWM_DUTY_MODE_0;

        mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm);
        mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm);

        rcCar_stop();
    }

    // Arrete les deux moteurs.
    void rcCar_stop() {

        duty_cycle_right = 0;
        duty_cycle_left = 0;

        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0);
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 0);
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, 0);
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, 0);
    }

    // Controle les moteurs avec le joystick.
    void rcCar_cmd(int x, int y) {

          // Limiter les valeurs du joystick.
        x = constrain(x, -100, 100);
        y = constrain(y, -100, 100);
        // Arret si la voiture est renversee.
        if (tiltDetected) {
            rcCar_stop();
            return;
        }

        // Arret si le joystick est au centre.
        if (abs(x) < 10 && abs(y) < 10) {
            rcCar_stop();
            return;
        }

        // Calcul des virages.
        turn_factor = 1.0 - pow(abs(x) / 100.0, 2);
        turn_factor = constrain(turn_factor, 0.9, 1.0);

        // Inverser la direction en reculant.
        adjusted_x = (y < -10) ? -x : x;

        // Calcul de la puissance des moteurs.
        duty_cycle_right = (y - adjusted_x * weight) * turn_factor;
        duty_cycle_left = (y + adjusted_x * weight) * turn_factor;

        // Limiter la puissance entre -100 et 100.
        duty_cycle_right = constrain(duty_cycle_right, -100, 100);
        duty_cycle_left = constrain(duty_cycle_left, -100, 100);

        // Moteur droit.
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A,
                       max(0, duty_cycle_right));
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B,
                       max(0, -duty_cycle_right));

        // Moteur gauche.
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A,
                       max(0, -duty_cycle_left));
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B,
                       max(0, duty_cycle_left));
    }

  // Utilise les valeurs du joystick heritees de Commande.
    void appliquerCommande() {
        rcCar_cmd(manettes.jRX, manettes.jRY);
    }
    // Arrete les moteurs si la voiture se renverse.
    void verifierInclinaison(bool estRenverse) {

        tiltDetected = estRenverse;

        if (tiltDetected) {
            rcCar_stop();
        }
    }
};

#endif // MOTEUR_H
