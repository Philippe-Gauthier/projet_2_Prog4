/*
  ESP32 RC Car with IMU Tilt Detection
  Features:
    - MCPWM motor control (2 DC motors)
    - ESP-NOW wireless communication
    - BMI323 IMU with tilt detection on INT1 interrupt
    - NeoPixel RGB LED indicators
    - Battery monitoring and voltage display
    - Motor safety cutoff when device is tilted

  Based upon Espressif ESP32CAM Examples
  LP Gauthier 2025
*/

// ==================== INCLUDE FILES ====================

// -------- Arduino/ESP32 Standard Libraries --------
#include "Arduino.h"              // Core Arduino functions and types
#include "soc/soc.h"              // ESP32 System-on-Chip low-level definitions
#include "soc/rtc_cntl_reg.h"     // ESP32 RTC control register definitions

// -------- ESP32 Wireless & Communication --------
#include <esp_now.h>              // ESP-NOW wireless protocol for remote control
#include <WiFi.h>                 // WiFi library (required by esp_now)
#include <Wire.h>                 // I2C communication protocol

// -------- Third-Party Libraries --------
#include <ArduinoJson.h>          // JSON parsing for remote control messages
#include "Adafruit_NeoPixel.h"    // RGB LED strip control library

// -------- Custom Libraries & Configuration --------
#include "user_define.h"          // Configuration constants and pin definitions
#include "rc_car.h"               // RC car helper functions (motor, battery, comms)

// ==================== GLOBAL STATE VARIABLES ====================

// LED state management
bool LED_STATE = false;
bool IMU_ERROR = true; // Assume IMU error until successfully initialized

// SELECT button state tracking (for button press detection)
bool previousSeState = true;
bool currentSeState = true;

// Tilt detection state for motor safety
bool tiltDetected = false;

// Time constants for sensor reading intervals
static unsigned long lastIMUReading = 0;
static unsigned long IMUInterval = 50; // 50ms

static unsigned long lastBatteryCheck = 0;
static unsigned long BatteryCheckInterval = 30000; // 30 seconds


// ==================== PERIPHERAL OBJECTS ====================

// NeoPixel RGB LED strips
Adafruit_NeoPixel pixelsBattery(NEOPIXEL_BATTERY_NUMBER, NEOPIXEL_BATTERY_PIN, NEO_GRB + NEO_KHZ800); // Battery status indicator

// ==================== DATA STRUCTURES ====================

// JSON message structure for ESP-NOW protocol
typedef struct struct_message {
  char command[256]; // JSON command buffer containing button/joystick data
} struct_message;

struct_message incomingMessage;

// ==================== REMOTE DEVICE CONFIGURATION ====================

// MAC address of the remote controller (phone/gamepad)
// Format: {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX}
uint8_t peerAddress[] = {0xDC, 0x54, 0x75, 0xC0, 0x83, 0xCC};

// ==================== SETUP ====================

/**
 * Initialization function - runs once at startup
 * Configures:
 *   - Motor PWM control
 *   - Serial communication
 *   - WiFi and ESP-NOW protocol
 *   - Remote peer device
 */
void setup() {
  // Disable brownout detector to prevent unexpected resets under high current draw
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  // Initialize motor control PWM
  rcCar_setup();

  // Initialize NeoPixel battery indicator
  pixelsBattery.begin();
  pixelsBattery.setBrightness(NEOPIXEL_BRIGHTNESS);
  pixelsBattery.fill(0x0000FF); // Fill blue to indicate startup
  pixelsBattery.show();

  // Initialize serial communication (115200 baud)
  Serial.begin(115200);
  if (DEBUG) {
    Serial.setDebugOutput(true);
    //while(!Serial); // Wait for serial port to connect (for native USB devices)
    delay(3000);
  } else {
    Serial.setDebugOutput(false);
  }
        
  // ==================== ESP-NOW INITIALIZATION ====================
  
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

  // ==================== REGISTER REMOTE PEER ====================
  
  // Configure peer (remote controller) information
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA; // Use station mode interface

  // Display peer MAC address
  Serial.print("Adding peer with MAC: ");
  for (int i = 0; i < 6; i++) {
      Serial.printf("%02X", peerAddress[i]);
      if (i < 5) Serial.print(":");
  }
  Serial.println();

  // Add peer if it doesn't exist
  if (!esp_now_is_peer_exist(peerAddress)) {
      if (esp_now_add_peer(&peerInfo) != ESP_OK) {
          Serial.println("Failed to add peer");
          pixelsBattery.fill(0x0000FF); // Blue indicates peer registration error
          pixelsBattery.show();
          delay(2000);
          ESP.restart();
          return;
      }
  } else {
      Serial.println("Peer already exists");
  }

  // ==================== STARTUP INDICATION ====================
  // Perform initial battery check
  Serial.println("Performing initial battery check...");
  getBatteryPercentage();

  Serial.println("Setup complete, entering main loop...");
  // Blink LED 3 times to indicate successful setup
  for (int i = 0; i < 6; i++) {
    toggleLights();
    delay(100);
  }
}

// ==================== MAIN LOOP ====================
/**
 * Main program loop - runs continuously
 * Handles:
 *   - ESP-NOW message processing
 *   - Battery voltage monitoring (every 30 seconds)
 */
void loop() {
  // Check battery voltage every 30 seconds
  if (millis() - lastBatteryCheck > BatteryCheckInterval) {
    lastBatteryCheck = millis();
    getBatteryPercentage();
  }
}