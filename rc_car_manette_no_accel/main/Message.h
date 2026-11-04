#ifndef MESSAGE_H
#define MESSAGE_H

#include <WiFi.h>
#include <esp_now.h>
#include <user_define.h>

struct struct_message
{
    char data[256];
    int len;

};

class message {
    public:
        struct_message incomingMessage;
        uint8_t peerAddress[6] = {0xDC, 0x54, 0x75, 0xC0, 0x83, 0xCC};
        void initialization_esp(void (*onDataRecv)(const esp_now_recv_info_t *, const uint8_t *, int )){
                    // Initialize WiFi in station mode for ESP-NOW
                    WiFi.mode(WIFI_STA);
                    Serial.print("ESP32 MAC Address: ");
                    Serial.println(WiFi.macAddress());
                    peerInfo.channel = 0;  
                    peerInfo.encrypt = false;
                    peerInfo.ifidx = WIFI_IF_STA;

                    // Initialize ESP-NOW protocol
                    if (esp_now_init() != ESP_OK) {
                        Serial.println("Error initializing ESP-NOW");
                        delay(2000);
                        ESP.restart(); // Restart device if ESP-NOW fails
                        return;
                    }

                    // Register callback function for incoming ESP-NOW messages
                    esp_now_register_recv_cb(onDataRecv);
                }

    private:
        esp_now_peer_info_t peerInfo = {};
        // Function that initializes the ESP-NOW to communicate with the ESP
        
        // Function that maintains the communication
        void maintenance_communication(){

        }
};

#endif