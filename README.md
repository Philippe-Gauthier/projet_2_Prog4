# projet_2_Prog4


### rccar_batterie
__Attribut__
* Publique :
    * float tension
    * int pourcentage : (entre 0 et 100)
    * int pin
    * int threshold_r
    * int threshold_j
    * float max
    * float min
* Privé :
    * int valeur_raw
    * int resolution

__Methodes__
* Publique :
    * update_values() : void 
    * get_threshold() : int (0 | 1 | 2)
    * rccar_batterie(int new_pin, float n_min, float n_max, int threshold1, int threshold2) : void
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
  
## Classe Information

### Rôle

La classe `Information` récupère les données de la batterie et l'état de l'accéléromètre, les regroupe dans une structure `Donnees`, les convertit en un seul message JSON et déclenche son envoi.

La classe `Information` hérite de la classe `message`.

### Attributs

| Attribut | Type | Visibilité | Description |
|---|---|---|---|
| `batterie` | `DonneesBatterie` | Public | Contient la tension et le pourcentage de la batterie. |
| `accelerometre` | `DonneesAccelerometre` | Public | Contient l'état d'erreur de l'accéléromètre. |

### Structures utilisées

**DonneesBatterie**
- `tension` : `float` — Tension de la batterie.
- `pourcentage` : `int` — Pourcentage de la batterie.

**DonneesAccelerometre**
- `state` : `bool` — État d'erreur de l'accéléromètre (`true` = erreur, `false` = aucune erreur).

**Donnees**
- `batterie` : `DonneesBatterie`
- `accelerometre` : `DonneesAccelerometre`

Cette structure regroupe les informations des deux capteurs.

### Méthodes

**Publique : `envoie() : bool`**

- Récupère les valeurs des attributs `batterie` et `accelerometre`.
- Regroupe les valeurs dans la structure `Donnees`.
- Appelle la méthode privée `encodage()`.
- Demande l'envoi du JSON.
- Retourne `true` si la demande d'envoi est acceptée, sinon `false`.

**Privée : `encodage(const Donnees& donnees) : String`**

- Reçoit une structure `Donnees`.
- Convertit les informations de la batterie et de l'accéléromètre en JSON.
- Retourne le JSON sous forme de `String`.

### Relations

- **Batterie → Information** : écrit la tension et le pourcentage dans l'attribut `batterie`.
- **Accéléromètre → Information** : écrit son état d'erreur dans l'attribut `accelerometre`.
- **Information → message** : hérite de la classe `message` et utilise l'adresse MAC héritée pour demander l'envoi du JSON.

### Fonctionnement

1. La classe Batterie met à jour l'attribut `batterie`.
2. La classe Accéléromètre met à jour l'attribut `accelerometre`.
3. La méthode `envoie()` regroupe les attributs dans la structure `Donnees`.
4. La méthode `encodage()` transforme cette structure en un seul JSON.
5. La méthode `envoie()` demande la transmission du JSON.

### Exemple de JSON

```json
{
  "batterie": {
    "tension": 3.9,
    "pourcentage": 80
  },
  "accelerometre": {
    "state": false
  }
}
```


  
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
  - `bool tiltDetected` : Indique si la voiture est renversée (true) ou non (false).

## Méthodes

- **Public :**
  - `void rcCar_setup()` : Initialise les broches et le PWM des moteurs, ainsi que les phares et le NeoPixel.
  - `void rcCar_stop()` : Arrête les deux moteurs en mettant les quatre sorties PWM à 0.
  - `void rcCar_cmd(int x, int y)` : Calcule la vitesse et la direction des moteurs selon les valeurs du joystick.
    - `x` : Direction gauche/droite.
    - `y` : Déplacement avant/arrière.
  -`void verifierInclinaison()` : Vérifie automatiquement si la voiture est renversée et arrête les moteurs avec rcCar_stop() si nécessaire.

- **Privé :**
  - Aucune méthode.


## Jérémy : Accéléromètre
* class Accéléromètre : Information {

## Attributs
Private:
* std::list<float> Donnée_Brute = [1.0, 2.0, 3.0]
* float Threshold = 50.0
* struct {
  float Accel_X;
  float Accel_Y;
  float Accel_Z;
} Acceleration_data;
  
## Méthodes
Private:
```
void Accel_init() {
  démarrage ligne I2C et mise au point des registres au besoin
}
```

```
  list<float> Get_values() { // lecture des registres
    Value_list = [1.0, 2.0, 3.0];
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
    print(message de test);
}
```
Public:
```
  void Set_Threshold(float tilt) {
    this->Threshold = tilt;
    }
```
```
// utilise la structure dans les attributs
  bool Error_check(float thresh, Acceleration_data) {
    if (data[x] > thresh):
      return True
    else:
      return False
  }
```
    

## Lumière

### Attributs

* Publique

  * `bool LED_STATE; // variable utilisée pour la fonction togglelights`
  * `int Neopixel_brightness; // utilisé durant l'initialisation`
  * `int lights_power; // 0 à 100`
* Privé

  * `const int lights_pwm_channel = 0; // utilisé par write2LED`
  * `Adafruit_NeoPixel pixelsBattery(int NEOPIXEL_BATTERY_NUMBER, int NEOPIXEL_BATTERY_PIN, NEO_GRB + NEO_KHZ800);`
  * `const int Neopixel_battery_pin;`
  * `const int Neopixel_battery_number;`
  * `const int lights_pin;`

### Méthode

* Publique

  * `void toggleLights(); allume ou ferme les lumières (cette fonction utilise l'intensité des lumières quand actif) | utilise la variable lights_power comme paramètre d'intensité`
  * `void write2LED(int pourcentage (entre 1 et 100));`
  * `int updateBatteryLED(int batteryLED_level); // Fonction qui met à jour la couleur de la batterie et la valeur "battery_state" avec la valeur retournée`
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
    * void initialisation()
    * void stop()
    * void update()
* Privé :
    * envoie_message_commande(char command[])
    * reception_message_mannette()
    * void gererLumieres()
    * void gererMoteurs()
    * void gererAccelerometre()
    * void gererBatterie()



## Message

### Attributs
**Public**:
* `extern struct_message incomingMessage`
* `uint8_t peerAddress[]`
    
**Privé**:
* `longueur_message`
* `esp_now_peer_info_t peerInfo = {};`
* `memcpy(peerInfo.peer_addr, peerAddress, 6);`
* `peerInfo.channel = 0;`
* `peerInfo.encrypt = false;`
* `peerInfo.ifidx = WIFI_IF_STA;`

### Méthodes
**Public**:
* `esp_now_register_recv_cb(onDataRecv);`

**Privé**:
* `void initialization_esp()`
* `void initialization_serial_communication()`
* `void maintenance_communication()`
* `void peer_communication()`


