#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "user_define.h"

class message
{
public:
    uint8_t peerAddress[6] = {
        0xDC, 0x54, 0x75, 0xC0, 0x83, 0xCC
    };

    // Initialisation de la communication ESP-NOW
    void initialization_esp(esp_now_recv_cb_t onDataRecv)
    {
        WiFi.mode(WIFI_STA);

        Serial.print("ESP32 MAC Address: ");
        Serial.println(WiFi.macAddress());

        if (esp_now_init() != ESP_OK)
        {
            Serial.println("Error initializing ESP-NOW");
            delay(2000);
            ESP.restart();
            return;
        }

        // Enregistrement du callback du main
        esp_now_register_recv_cb(onDataRecv);

        // Configuration du destinataire
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        peerInfo.ifidx = WIFI_IF_STA;

        memcpy(peerInfo.peer_addr, peerAddress, 6);

        if (!esp_now_is_peer_exist(peerAddress))
        {
            if (esp_now_add_peer(&peerInfo) != ESP_OK)
            {
                Serial.println("Erreur ajout destinataire ESP-NOW");
            }
        }
    }

    // Initialisation du moniteur série
    void initialization_serial_communication()
    {
        Serial.begin(115200);

        if (DEBUG)
        {
            Serial.setDebugOutput(true);
            delay(3000);
        }
        else
        {
            Serial.setDebugOutput(false);
        }
    }

    // Maintien de la communication
    void maintenance_communication()
    {
    }

private:
    int longueur_message = 0;

    esp_now_peer_info_t peerInfo = {};
};

#endif