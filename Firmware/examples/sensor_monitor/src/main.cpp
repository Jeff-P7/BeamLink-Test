#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include "BeamLink.h"
#include "BeamConfig.h"
#include "BeamUtils.h"
#include "../data/beam.config.h"

// Simulate sensor readings (replace with real sensors in production)
float readTemperature() {
  return 20.0 + (random(0, 100) / 10.0); // 20-30°C
}

float readHumidity() {
  return 40.0 + (random(0, 200) / 10.0); // 40-60%
}

int readLightLevel() {
  return random(0, 1024); // 0-1023
}

BeamConfig config;
BeamLink beam;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== BeamLink Sensor Monitor ===\n");
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed!");
    return;
  }
  
  // Load configuration
  if (!loadBeamConfig(config)) {
    Serial.println("Using default configuration");
  }
  
  // Initialize BeamLink with compile-time constants from beam.config.h
  if (!beam.begin(BLE_NAME, BLE_POWER_DBM, BLE_ADV_INTERVAL_MS,
                  BLE_SERVICE_UUID, BLE_RX_UUID, BLE_TX_UUID)) {
    Serial.println("Failed to initialize BeamLink!");
    return;
  }
  
  // Set up comprehensive message handler
  beam.onMessage([](const std::string& msg, ReplyFn reply) {
    using namespace BeamUtils;
    
    std::string cmd, action, value;
    
    // Handle simple commands
    if (msg == "help") {
      reply("Commands: temp, humidity, light, stats, all, uptime, reset, help");
    }
    else if (msg == "temp") {
      float temp = readTemperature();
      reply("Temperature: " + std::to_string(temp) + "°C");
    }
    else if (msg == "humidity") {
      float hum = readHumidity();
      reply("Humidity: " + std::to_string(hum) + "%");
    }
    else if (msg == "light") {
      int light = readLightLevel();
      reply("Light: " + std::to_string(light) + "/1023");
    }
    else if (msg == "all") {
      // Send all sensor readings
      std::string data = "Temp=" + std::to_string(readTemperature()) + "°C";
      data += ", Hum=" + std::to_string(readHumidity()) + "%";
      data += ", Light=" + std::to_string(readLightLevel());
      reply(data);
    }
    else if (msg == "stats") {
      std::string stats = formatStats(
        beam.getMessagesReceived(),
        beam.getMessagesSent(),
        beam.getErrors(),
        beam.getUptime()
      );
      reply(stats);
    }
    else if (msg == "uptime") {
      reply("Uptime: " + formatUptime(beam.getUptime()));
    }
    else if (msg == "reset") {
      beam.resetStats();
      reply("Statistics reset");
    }
    else if (msg == "mtu") {
      reply("MTU: " + std::to_string(beam.getMTU()) + " bytes");
    }
    else if (msg == "info") {
      std::string info = std::string(DEVICE_NAME) + " (" + std::string(DEVICE_ID) + ") ";
      info += "FW:" + std::string(FIRMWARE_VERSION);
      reply(info);
    }
    // Handle command:action format
    else if (parseCommand(msg, cmd, action)) {
      if (cmd == "config") {
        if (action == "name") {
          reply("Device: " + std::string(DEVICE_NAME));
        } else if (action == "id") {
          reply("ID: " + std::string(DEVICE_ID));
        } else if (action == "fw") {
          reply("Firmware: " + std::string(FIRMWARE_VERSION));
        } else {
          reply("Unknown config: " + action);
        }
      }
      else if (cmd == "get") {
        if (action == "temp") {
          reply(std::to_string(readTemperature()));
        } else if (action == "hum") {
          reply(std::to_string(readHumidity()));
        } else if (action == "light") {
          reply(std::to_string(readLightLevel()));
        } else {
          reply("Unknown sensor: " + action);
        }
      }
      else {
        reply("Unknown command: " + cmd);
      }
    }
    // Handle key=value format
    else if (msg.find('=') != std::string::npos) {
      auto params = parseKeyValue(msg);
      std::string response = "Parsed " + std::to_string(params.size()) + " parameters: ";
      for (const auto& pair : params) {
        response += pair.first + "=" + pair.second + " ";
      }
      reply(response);
    }
    else {
      reply("Unknown command. Type 'help' for commands.");
    }
  });
  
  Serial.println("\n=== Sensor Monitor Ready ===");
  Serial.println("Available commands:");
  Serial.println("  - temp, humidity, light, all");
  Serial.println("  - stats, uptime, reset, mtu");
  Serial.println("  - info, help");
  Serial.println("  - config:name, config:id, config:fw");
  Serial.println("  - get:temp, get:hum, get:light");
  Serial.println("================================\n");
}

void loop() {
  beam.loop();
  
  // Optional: Auto-send sensor readings every 30 seconds
  static unsigned long lastSend = 0;
  unsigned long now = millis();
  
  if (beam.isConnected() && (now - lastSend > 30000)) {
    std::string data = "Auto: Temp=" + std::to_string(readTemperature()) + "°C";
    data += ", Hum=" + std::to_string(readHumidity()) + "%";
    data += ", Light=" + std::to_string(readLightLevel());
    beam.notify(data);
    lastSend = now;
  }
  
  delay(10);
}

