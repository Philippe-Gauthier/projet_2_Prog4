#ifndef INFORMATION_H
#define INFORMATION_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Structure contenant les données des capteurs
struct Donnees
{
    float tension;
    int pourcentage;

    float Accel_X;
    float Accel_Y;
    float Accel_Z;
};

class Information
{
private:

    // Convertir la structure en JSON.
    String encodage(const Donnees& donnees)
    {
        JsonDocument doc;

        doc["tension"] = donnees.tension;
        doc["pourcentage"] = donnees.pourcentage;

        doc["Accel_X"] = donnees.Accel_X;
        doc["Accel_Y"] = donnees.Accel_Y;
        doc["Accel_Z"] = donnees.Accel_Z;

        String json;
        serializeJson(doc, json);

        return json;
    }

public:

    // Encoder les données et préparer leur envoi
    bool envoie(const Donnees& donnees)
    {
        String json = encodage(donnees);

        if (json.length() == 0)
        {
            return false;
        }

        // Envoi à intégrer avec la classe Message
        Serial.println(json);

        return true;
    }
};

#endif