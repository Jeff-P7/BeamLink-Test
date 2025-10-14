#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include "BeamLink.h"
#include "BeamConfig.h"

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
  pinMode(config.ledPin, OUTPUT);
  digitalWrite(config.ledPin, config.ledActiveHigh ? LOW : HIGH);
  Serial.printf("LED initialized on pin %d\n", config.ledPin);
  
  // Initialize BeamLink BLE with configuration
  if (!beam.begin(config.deviceName.c_str(), config.advPowerDbm, config.advIntervalMs)) {
    Serial.println("Failed to initialize BeamLink");
    return;
  }
  
  // Set up message handler for LED control
  beam.onMessage([&](const std::string& in, ReplyFn reply) {
    if (in == "led:on") {
      digitalWrite(config.ledPin, config.ledActiveHigh ? HIGH : LOW);
      reply("LED ON");
      Serial.println("LED turned ON");
    } 
    else if (in == "led:off") {
      digitalWrite(config.ledPin, config.ledActiveHigh ? LOW : HIGH);
      reply("LED OFF");
      Serial.println("LED turned OFF");
    } 
    else if (in == "led:status") {
      bool isOn = digitalRead(config.ledPin) == (config.ledActiveHigh ? HIGH : LOW);
      reply(isOn ? "LED ON" : "LED OFF");
      Serial.printf("LED status: %s\n", isOn ? "ON" : "OFF");
    }
    else if (in == "led:toggle") {
      bool isOn = digitalRead(config.ledPin) == (config.ledActiveHigh ? HIGH : LOW);
      digitalWrite(config.ledPin, config.ledActiveHigh ? !isOn : isOn);
      reply(isOn ? "LED OFF" : "LED ON");
      Serial.printf("LED toggled to: %s\n", isOn ? "OFF" : "ON");
    }
    else if (in == "info") {
      std::string info = "Device: " + config.deviceName + 
                        ", ID: " + config.deviceId +
                        ", Type: " + config.deviceType +
                        ", FW: " + config.fwVersion;
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

