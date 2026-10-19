
#ifndef INFORMATION_H
#define INFORMATION_H

#include <Arduino.h>
#include <ArduinoJson.h>

// ==========================================
// STRUCTURE DES DONNEES DE LA BATTERIE
// ==========================================

struct DonneesBatterie
{
    float tension = 0;
    int pourcentage = 0;
};

// ==========================================
// STRUCTURE DES DONNEES DE L'ACCELEROMETRE
// ==========================================

struct DonneesAccelerometre
{
    bool state = false;
};

// ==========================================
// STRUCTURE PRINCIPALE
// ==========================================

struct Donnees
{
    DonneesBatterie batterie;
    DonneesAccelerometre accelerometre;
};

// ==========================================
// CLASSE INFORMATION
// ==========================================

class Information
{
private:

    // Convertit toutes les donnees en JSON
    String encodage(const Donnees& donnees)
    {
        JsonDocument doc;

        // Donnees de la batterie
        doc["batterie"]["tension"] =
            donnees.batterie.tension;

        doc["batterie"]["pourcentage"] =
            donnees.batterie.pourcentage;

        // Etat de l'accelerometre
        doc["accelerometre"]["state"] =
            donnees.accelerometre.state;

        // Creation du JSON
        String json;
        serializeJson(doc, json);

        return json;
    }

public:

    // Encode les donnees et prepare leur envoi
    bool envoie(const Donnees& donnees)
    {
        // Appel de la methode privee
        String json = encodage(donnees);

        // Verification du JSON
        if (json.length() == 0)
        {
            return false;
        }

        // Affichage pour les tests
        Serial.println(json);

        // Appeler la methode d'envoi
        // de la classe Message (ESP-NOW).

        // Pour l'instant true signifie
        // seulement que le JSON a ete cree.
        return true;
    }
};

#endif
