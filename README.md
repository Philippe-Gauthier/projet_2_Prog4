# projet_2_Prog4

### batterie
__Attribue__
* Publique :
    * float tension
    * int pourcentage : (entre 0 et 100)
    * int pin
    * int threshold_r
    * int threshold_j
    * int max
    * int min
    * lumière led
* Privé :
    * uint12_t valeur_raw
    * int resolution

__Methodes__
* Publique :
    * update_values() : void 
    * get_threshold() : int (0 | 1 | 2)
    * init() : void
    * updateBatteryLED(): void
* Privé :
    * convert_raw() : float
