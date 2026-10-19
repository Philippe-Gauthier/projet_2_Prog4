#include <main.h>

class message {
    public:
        struct_message incomingMessage;
        uint8_t peerAddress[] = {0xDC, 0x54, 0x75, 0xC0, 0x83, 0xCC};
        esp_now_register_recv_cb(onDataRecv);
    
    private:
        int longueur_message;

        void initialization_esp(){
            // Initialize WiFi in station mode for ESP-NOW
            WiFi.mode(WIFI_STA);
            Serial.print("ESP32 MAC Address: ");
            Serial.println(WiFi.macAddress());
  
            // Initialize ESP-NOW protocol
            if (esp_now_init() != ESP_OK) {
                Serial.println("Error initializing ESP-NOW");
                pixelsBattery.fill(0x0000FF); // Blue color indicates ESP-NOW error
                pixelsBattery.show();
                delay(2000);
                ESP.restart(); // Restart device if ESP-NOW fails
                return;
            }
  
            // Register callback function for incoming ESP-NOW messages
            esp_now_register_recv_cb(onDataRecv);
        }

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
};