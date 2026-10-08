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

  

### Commande

- **Mère :** `message`
- **Enfant 1 :** `moteur`
- **Enfant 2 :** `lumière`

## Attributs

- * Public :
  - `char command[256]` : commande reçue

- * Privé :
  - Aucun


## Méthodes
- * Public :
   - ` Joystick_Bouton() : String`
   - ` decoding_JSON(String message) : void`
- * Privé :
  - Aucun
  
### Information

Rôle : Récupérer les données de la batterie et de l'accéléromètre, les encoder en JSON et les transmettre à la classe Message.

## Attributs
| Nom | Type | Visibilité |
|---|---|---|
| `message` | `String` | Private |
| `longueurMessage` | `size_t` | Private |

## Méthodes
| Nom | Retour | Paramètres | Visibilité |
|---|---|---|---|
| `encodage_json()` | `String` | `const String& donnees` | Public |
| `envoie_message()` | `bool` | `const String& messageJson` | Public |

## Relations
- **Batterie → Information :** fournit les données de batterie.
- **Accéléromètre → Information :** fournit les données d'inclinaison.
- **Information → Message :** transmet le JSON préparé.
  
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
