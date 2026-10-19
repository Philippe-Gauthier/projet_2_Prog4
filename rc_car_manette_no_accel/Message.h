#include <main.h>

class message {
    public:
        struct_message incomingMessage;
        esp_now_register_recv_cb(onDataRecv);
        uint8_t peerAddress[] = {0xDC, 0x54, 0x75, 0xC0, 0x83, 0xCC};
    
    private:
        int longueur_message;
        esp_now_peer_info_t peerInfo = {};
        peerInfo.channel = 0;  
        peerInfo.encrypt = false;
        peerInfo.ifidx = WIFI_IF_STA;

        // Function that initializes the ESP-NOW to communicate with the ESP
        void initialization_esp(){
            // Initialize WiFi in station mode for ESP-NOW
            WiFi.mode(WIFI_STA);
            Serial.print("ESP32 MAC Address: ");
            Serial.println(WiFi.macAddress());
  
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

        // Function that initializes the serial communication
        void initialization_serial_communication(){
            // Initialize serial communication (115200 baud)
            Serial.begin(115200);
            if (DEBUG) {
                Serial.setDebugOutput(true);
                delay(3000);
            } else {
                Serial.setDebugOutput(false);
            }
        }

        // Function that maintains the communication
        void maintenance_communication(){

        }
};