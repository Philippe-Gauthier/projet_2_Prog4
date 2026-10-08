#include <Arduino.h>
#include "Information.h"

Information info;

void setup()
{
    Serial.begin(115200);

    String json = info.encodage_json("Batterie 80%");

    if (info.envoie_message(json))
    {
        Serial.println(json);
    }
}

void loop()
{
}