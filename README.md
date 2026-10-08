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

Aucun

## Méthodes

- `String Joystick_Bouton()`
- `void decoding_JSON(String message)`


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
  
