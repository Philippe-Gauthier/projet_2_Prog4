
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
public:

    // ===== ATTRIBUTS PUBLICS =====

    // Modifie par la classe Batterie
    DonneesBatterie batterie;

    // Modifie par la classe Accelerometre
    DonneesAccelerometre accelerometre;

private:

    // Convertit la structure complete en JSON
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

    // Regroupe les attributs et prepare le JSON
    bool envoie()
    {
        // Creation de la structure principale
        Donnees donnees;

        // Recuperation des attributs
        donnees.batterie = batterie;
        donnees.accelerometre = accelerometre;

        // Encodage des donnees en JSON
        String json = encodage(donnees);

        if (json.length() == 0)
        {
            return false;
        }

        // Affichage pour les tests
        Serial.println(json);

        // envoyer le JSON avec
        // la classe Message (ESP-NOW).

        // Pour l'instant true indique
        // uniquement que le JSON est genere.
        return true;
    }
};

#endif