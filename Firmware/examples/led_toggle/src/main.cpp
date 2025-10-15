#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include "BeamLink.h"
#include "BeamConfig.h"
#include "../data/beam.config.h"

BeamConfig config;
BeamLink beam;

void setup() {
  Serial.begin(115200);
  Serial.println("BeamLink LED Toggle Example");
  
  // Initialize SPIFFS for config file
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed!");
    return;
  }
  
  // Load configuration from beam.config
  if (!loadBeamConfig(config)) {
    Serial.println("Using default configuration");
  }
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? LOW : HIGH);
  Serial.printf("LED initialized on pin %d\n", LED_PIN);
  
  // Initialize BeamLink BLE with compile-time constants
  if (!beam.begin(BLE_NAME, BLE_POWER_DBM, BLE_ADV_INTERVAL_MS,
                  BLE_SERVICE_UUID, BLE_RX_UUID, BLE_TX_UUID)) {
    Serial.println("Failed to initialize BeamLink");
    return;
  }
  
  // Set up message handler for LED control
  beam.onMessage([&](const std::string& in, ReplyFn reply) {
    if (in == "led:on") {
      digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? HIGH : LOW);
      reply("LED ON");
      Serial.println("LED turned ON");
    } 
    else if (in == "led:off") {
      digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? LOW : HIGH);
      reply("LED OFF");
      Serial.println("LED turned OFF");
    } 
    else if (in == "led:status") {
      bool isOn = digitalRead(LED_PIN) == (LED_ACTIVE_HIGH ? HIGH : LOW);
      reply(isOn ? "LED ON" : "LED OFF");
      Serial.printf("LED status: %s\n", isOn ? "ON" : "OFF");
    }
    else if (in == "led:toggle") {
      bool isOn = digitalRead(LED_PIN) == (LED_ACTIVE_HIGH ? HIGH : LOW);
      digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? !isOn : isOn);
      reply(isOn ? "LED OFF" : "LED ON");
      Serial.printf("LED toggled to: %s\n", isOn ? "OFF" : "ON");
    }
    else if (in == "info") {
      std::string info = "Device: " + std::string(DEVICE_NAME) + 
                        ", ID: " + std::string(DEVICE_ID) +
                        ", Type: " + std::string(DEVICE_TYPE) +
                        ", FW: " + std::string(FIRMWARE_VERSION);
      reply(info);
      Serial.println("Info requested");
    }
    else {
      reply("Unknown Command");
      Serial.printf("Unknown command: %s\n", in.c_str());
    }
  });
  
  Serial.println("BeamLink-ESP32 ready.");
  Serial.println("Commands: led:on, led:off, led:status, led:toggle, info");
}

void loop() {
  beam.loop();
  delay(10);
}

