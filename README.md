# projet_2_Prog4


### batterie
__Attribut__
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

### Commande

- **Mère :** `message`
- **Enfant 1 :** `moteur`
- **Enfant 2 :** `lumière`

## Attributs

- **Public :**
- struct Commandes_Manettes
   - jRX : int — axe horizontal du joystick
   - jRY : int — axe vertical du joystick
   - Se : bool — état du bouton SELECT
   - U : bool — état du bouton U
   - D : bool — état du bouton D

- **Privé :**
  - Aucun

## Méthodes

- **Public :**
  - `reception_Message(const uint8_t *incomingData, int len) : String`
  - `decoding_JSON(String message) : void`

- **Privé :**
  - Aucun
  
### Information

Rôle : Récupérer les données de la batterie et de l'accéléromètre, les encoder en JSON et les transmettre à la classe Message.

## Attributs
Aucun (les attributs du message sont gérés par une autre classe).

## Méthodes

Publique :
envoie(const Donnees& donnees) : bool

Privée :
encodage(const Donnees& donnees) : String

## Relations
- **Batterie → Information :** fournit les données de batterie.
- **Accéléromètre → Information :** fournit les données d'inclinaison.

## Fonctionnement

encodage() : transforme les données d'une structure (struct) en JSON.
envoie() : appelle encodage(), transmet le JSON à la classe Message et retourne un booléen indiquant si la demande d'envoi a réussi.


  
# Moteur

## Attributs

- **Public :**
  - Aucun

- **Privé :**
  - `int duty_cycle_right` : Puissance et sens du moteur droit (entre -100 et 100).
  - `int duty_cycle_left` : Puissance et sens du moteur gauche (entre -100 et 100).
  - `float turn_factor` : Facteur de réduction de vitesse lors des virages (entre 0,9 et 1,0).
  - `int adjusted_x` : Ajuste la direction du joystick lorsque la voiture recule.
  - `float weight` : Influence du joystick X sur les moteurs (0,5).
  - `int RIGHT_MOTOR_FWD` : Broche 2, marche avant du moteur droit.
  - `int RIGHT_MOTOR_BWD` : Broche 45, marche arrière du moteur droit.
  - `int LEFT_MOTOR_FWD` : Broche 44, marche avant du moteur gauche.
  - `int LEFT_MOTOR_BWD` : Broche 42, marche arrière du moteur gauche.

## Méthodes

- **Public :**
  - `void rcCar_setup()` : Initialise les broches et le PWM des moteurs, ainsi que les phares et le NeoPixel.
  - `void rcCar_stop()` : Arrête les deux moteurs en mettant les quatre sorties PWM à 0.
  - `void rcCar_cmd(int x, int y)` : Calcule la vitesse et la direction des moteurs selon les valeurs du joystick.
    - `x` : Direction gauche/droite.
    - `y` : Déplacement avant/arrière.

- **Privé :**
  - Aucune méthode.


  # projet_2_Prog4


## Jérémy : Accéléromètre
* class Accéléromètre : Information {

## Attributs
Private:
* std::list<int> Donnée_Brute = [1, 2, 3]
* int Threshold = 50
* struct {
  float Accel_X;
  float Accel_Y;
  float Accel_Z;
} Acceleration_data;
* String Direction = " " // juste pour afficher dans le terminal, aucun effet sur les moteurs
  
## Méthodes
Private:
```
void Accel_init() {
  démarrage ligne I2C et mise au point des registres au besoin
}
```

```
  list<int> Get_values() { // lecture des registres
    Value_list = [];
    read all the registers;
    append into list;
    return Value_list;
    }
```
```
  // Modifier pour prendre conpte de la nouvelle structure
  void update_values(Get_values, ) { // Met les attributs à jour
    for ((i=0 and i<3)i++):
      this->Données_Brute = list;
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
    this->Direction = New_direction;
}
```
Public:
```
  void Set_Threshold(int tilt) {
    this->Threshold = tilt;
    }
```
```
  bool Error_check(int thresh, std::list<int> data) {
    if (data[x] > thresh):
      return True
    else:
      return False
  }
```

    

## Lumière

### Attributs

* Publique

  * `bool LED_STATE;`
  * `int Neopixel_brightness; // utilisé durant l'initialisation`
  * `int lights_power; // 0 à 100`
  * `int battery_state; // 0 à 3, int indicant le niveau de la batterie: (0: inconnue/erreur, 1: rouge, 2: jaune, 3: verte))`
* Privé

  * `const int lights_pwm_channel = 0; // utilisé par write2LED`
  * `Adafruit_NeoPixel pixelsBattery(int NEOPIXEL_BATTERY_NUMBER, int NEOPIXEL_BATTERY_PIN, NEO_GRB + NEO_KHZ800);`
  * `const int Neopixel_battery_pin;`
  * `const int Neopixel_battery_number;`
  * `const int BATTERY_RED_THRESHOLD;`
  * `const int BATTERY_YELLOW_THRESHOLD;`
  * `const int lights_pin;`

### Méthode

* Publique

  * `void toggleLights();`
  * `void write2LED(int pourcentage (entre 1 et 100));`
  * `int updateBatteryLED(int batteryPercentage); // Fonction qui met à jour la couleur de la batterie et la valeur "battery_state" avec la valeur retournée`
* Privé

  * `void setupLights();`
  * `static uint8_t percentToPWM(int percent); // est utilisé par write2LED`

## Robot

__Attribues__
* Publique :

    * 

* Privé : 
    * controleurMoteurs moteurs;
    * controleurLumieres lumieres;
    * moniteurBatterie batterie;
    * Accelerometre accelerometre;
    * bool robotActif;
    * bool tiltDetecte;


__Méthodes__
* Publique :
    * begin()
    * stop()
    * update()
* Privé :
    * envoie_message_commande()
    * reception_message_mannette()
    * gererLumieres()
    * gererMoteurs()
    * gererAccelerometre()
    * gererBatterie()


## Message
### Attributs
**Public**:
    * `char command[256]`
    * `extern struct_message incomingMessage`
    * `uint8_t peerAddress[] = {0xDC, 0x54, 0x75, 0xC0, 0x83, 0xCC}`
    
**Privé**:
    * `struct_message`
    * `longueur_message`

### Méthodes
**Public**:
    * `void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);`

**Privé**:
    * Aucun


