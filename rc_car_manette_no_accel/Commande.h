#ifndef COMMANDE_H
#define COMMANDE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <cstring>

//  STRUCTURE DES MANETTES 

struct Commandes_Manettes
{
    int jRX = 0;      // Axe horizontal du joystick
    int jRY = 0;      // Axe vertical du joystick

    bool Se = false;  // Bouton SELECT
    bool U = false;   // Bouton U
    bool D = false;   // Bouton D
};

//  CLASSE COMMANDE 

class Commande
{
    public:
         Commandes_Manettes manettes;

         String reception_Message(const uint8_t *incomingData, int len);
         void decoding_JSON(String message);
};

//  RECEPTION DU MESSAGE 

String Commande::reception_Message(const uint8_t *incomingData,int len)
{
    char buffer[256];

    // Vérification de la longueur des données reçues
    if (incomingData == nullptr || len <= 0 || len >= sizeof(buffer))
    {
        Serial.println("Error: Incoming data too large or invalid!");
        return "";
    }

    // Copie des données dans un tampon temporaire
    memcpy(buffer, incomingData, len);
    buffer[len] = '\0';

    return String(buffer);
}

//  DECODAGE JSON       

void Commande::decoding_JSON(String message)
{
    JsonDocument doc;

    // Décodage du message JSON
    DeserializationError error = deserializeJson(doc, message);

    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.f_str());
        return;
    }

    // Parcours des paires clé-valeur du JSON
    for (JsonPair kv : doc.as<JsonObject>())
    {
        const char* key = kv.key().c_str();

        //  LECTURE DES BOUTONS 

        if (kv.value().is<bool>())
        {
            bool etat = kv.value().as<bool>();

            if (strcmp(key, "Se") == 0)
            {
                manettes.Se = etat;
            }
            else if (strcmp(key, "U") == 0)
            {
                manettes.U = etat;
            }
            else if (strcmp(key, "D") == 0)
            {
                manettes.D = etat;
            }
        }

        // LECTURE DU JOYSTICK 

        else if (kv.value().is<int>())
        {
            int valeur = kv.value().as<int>();

            if (strcmp(key, "jRX") == 0)
            {
                manettes.jRX = valeur;
            }
            else if (strcmp(key, "jRY") == 0)
            {
                manettes.jRY = valeur;
            }
        }
    }
}

#endif
