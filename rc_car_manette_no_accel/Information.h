/*
 * Fichier      : Information.h
 * Classe       : Information
 * Auteurs      : Said Tarzalt
 * Description  : Recupere les donnees de la batterie et
 *                de l'accelerometre, les regroupe dans
 *                une structure, les convertit en JSON
 *                et les envoie avec ESP-NOW.
 */

#ifndef INFORMATION_H
#define INFORMATION_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <esp_now.h>
#include "Message.h"

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

class Information : public message
{
public:

    // Attribut modifie par la batterie
    DonneesBatterie batterie;

    // Attribut modifie par l'accelerometre
    DonneesAccelerometre accelerometre;

private:

/*
* Fonction   : encodage
* Description: Convertit les donnees de la batterie
*              et de l'accelerometre en un seul JSON.
* Parametre  : donnees - Structure contenant les
*              informations des deux capteurs.
* Retour     : String contenant le JSON genere.
*/

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

/*
* Fonction   : envoie
* Description: Regroupe les informations des deux
*              capteurs, les encode en JSON et
*              demande leur envoi avec ESP-NOW.
* Parametre  : Aucun.
* Retour     : true si ESP-NOW accepte la demande
*              d'envoi, false sinon.
*/

    bool envoie()
    {
        // Regroupement des attributs
        Donnees donnees;

        donnees.batterie = batterie;
        donnees.accelerometre = accelerometre;

        // Encodage en JSON
        String json = encodage(donnees);

        // Verification du JSON
        if (json.length() == 0)
            {
                return false;
            }

        // Affichage pour les tests
        Serial.println(json);

        // Envoi avec l'adresse MAC heritee
        esp_err_t resultat = esp_now_send(
            peerAddress,
            reinterpret_cast<const uint8_t*>(json.c_str()),
            json.length()
        );

        return resultat == ESP_OK;
    }
};

#endif