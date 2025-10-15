#include <Arduino.h>
#include "BeamLink.h"
#include "BeamLog.hpp"
#include "../data/beam.config.h"

BeamLink beam;

void setup() {
  Serial.begin(115200);
  delay(50);

  LOG_INFO("BeamLink LED Toggle Example booting…");
  
  LOG_CFG("Using config: name=%s id=%s ledPin=%d", 
          DEVICE_NAME, DEVICE_ID, LED_PIN);
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? LOW : HIGH);
  LOG_PIN("GPIO%d set as OUTPUT (default OFF)", LED_PIN);
  
  // Initialize BeamLink BLE with compile-time constants
  if (!beam.begin(BLE_NAME, BLE_POWER_DBM, BLE_ADV_INTERVAL_MS,
                  BLE_SERVICE_UUID, BLE_CHARACTERISTIC_UUID)) {
    LOG_ERR("Failed to initialize BeamLink");
    return;
  }
  
  LOG_BLE("Advertising as %s", BLE_NAME);
  LOG_BLE("Service UUID: %s", BLE_SERVICE_UUID);
  LOG_BLE("Characteristic UUID: %s", BLE_CHARACTERISTIC_UUID);
  
  // Set up message handler for LED control
  beam.onMessage([&](const std::string& in, ReplyFn reply) {
    LOG_BLE("RX: %s", in.c_str());
    
    if (in == "led:on") {
      digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? HIGH : LOW);
      reply("LED ON");
      LOG_OK("LED turned ON");
    } 
    else if (in == "led:off") {
      digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? LOW : HIGH);
      reply("LED OFF");
      LOG_OK("LED turned OFF");
    } 
    else if (in == "led:status") {
      bool isOn = digitalRead(LED_PIN) == (LED_ACTIVE_HIGH ? HIGH : LOW);
      reply(isOn ? "LED ON" : "LED OFF");
      LOG_INFO("LED status: %s", isOn ? "ON" : "OFF");
    }
    else if (in == "led:toggle") {
      bool isOn = digitalRead(LED_PIN) == (LED_ACTIVE_HIGH ? HIGH : LOW);
      digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? !isOn : isOn);
      reply(isOn ? "LED OFF" : "LED ON");
      LOG_OK("LED toggled to: %s", isOn ? "OFF" : "ON");
    }
    else if (in == "info") {
      std::string info = "Device: " + std::string(DEVICE_NAME) + 
                        ", ID: " + std::string(DEVICE_ID) +
                        ", Type: " + std::string(DEVICE_TYPE) +
                        ", FW: " + std::string(FIRMWARE_VERSION);
      reply(info);
      LOG_INFO("Info requested");
    }
    else {
      reply("Unknown Command");
      LOG_WARN("Unknown command: %s", in.c_str());
    }
  });
  
  LOG_OK("Ready. Commands: led:on, led:off, led:status, led:toggle, info");
}

void loop() {
  beam.loop();
  delay(10);
}

