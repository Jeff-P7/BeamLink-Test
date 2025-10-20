#include <Arduino.h>
#include "BeamLink.h"
#include "BeamUtils.h"
#include "../include/beam.config.h"

BeamLink beam;

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

void setup() {
  // Initialize serial
  Serial.begin(SERIAL_BAUD);
  
  log_info("BeamLink Sensor Monitor Starting...");
  log_config("Device: " + String(DEVICE_NAME) + " (" + String(DEVICE_ID) + ")");
  
  // Initialize BeamLink BLE with constants
  if (!beam.begin(BLE_NAME, BLE_POWER_DBM, BLE_ADV_INTERVAL_MS,
                  BLE_SERVICE_UUID, BLE_CHARACTERISTIC_UUID)) {
    log_err("Failed to initialize BeamLink");
    return;
  }
  
  log_ble("BLE initialized successfully");
  
  // Set up comprehensive message handler
  beam.onMessage([](const std::string& msg, ReplyFn reply) {
    using namespace BeamUtils;
    
    std::string cmd, action, value;
    
    // Handle simple commands
    if (msg == "help") {
      reply("Commands: temp, humidity, light, stats, all, uptime, reset, help");
      log_info("Help requested");
    }
    else if (msg == "temp") {
      float temp = readTemperature();
      reply("Temperature: " + std::to_string(temp) + "°C");
      log_sensor("Temperature: " + String(temp) + "°C");
    }
    else if (msg == "humidity") {
      float hum = readHumidity();
      reply("Humidity: " + std::to_string(hum) + "%");
      log_sensor("Humidity: " + String(hum) + "%");
    }
    else if (msg == "light") {
      int light = readLightLevel();
      reply("Light: " + std::to_string(light) + "/1023");
      log_sensor("Light: " + String(light) + "/1023");
    }
    else if (msg == "all") {
      // Send all sensor readings
      std::string data = "Temp=" + std::to_string(readTemperature()) + "°C";
      data += ", Hum=" + std::to_string(readHumidity()) + "%";
      data += ", Light=" + std::to_string(readLightLevel());
      reply(data);
      log_sensor("All sensor readings sent");
    }
    else if (msg == "stats") {
      std::string stats = formatStats(
        beam.getMessagesReceived(),
        beam.getMessagesSent(),
        beam.getErrors(),
        beam.getUptime()
      );
      reply(stats);
      log_info("Statistics requested");
    }
    else if (msg == "uptime") {
      reply("Uptime: " + formatUptime(beam.getUptime()));
      log_info("Uptime requested");
    }
    else if (msg == "reset") {
      beam.resetStats();
      reply("Statistics reset");
      log_info("Statistics reset");
    }
    else if (msg == "mtu") {
      reply("MTU: " + std::to_string(beam.getMTU()) + " bytes");
      log_info("MTU requested");
    }
    else if (msg == "info") {
      std::string info = std::string(DEVICE_NAME) + " (" + std::string(DEVICE_ID) + ") FW:" + std::string(FIRMWARE_VERSION);
      reply(info);
      log_info("Device info requested");
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
          log_config("Config query: " + String(cmd.c_str()) + ":" + String(action.c_str()));
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
          log_sensor("Sensor query: " + String(action.c_str()));
        }
        else {
          reply("Unknown command: " + cmd);
          log_warn("Unknown command: " + String(cmd.c_str()));
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
      log_info("Key-value parsing: " + String(params.size()) + " parameters");
    }
    else {
      reply("Unknown command. Type 'help' for commands.");
      log_warn("Unknown command: " + String(msg.c_str()));
    }
  });
  
  log_success("Sensor Monitor Ready!");
  log_info("Available commands: temp, humidity, light, all, stats, uptime, reset, help");
}

void loop() {
  beam.loop();
  
  // Optional: Auto-send sensor readings every configured interval
  static unsigned long lastSend = 0;
  unsigned long now = millis();
  
  if (beam.isConnected() && (now - lastSend > REPORT_INTERVAL_MS)) {
    std::string data = "📊 Auto: Temp=" + std::to_string(readTemperature()) + "°C";
    data += ", Hum=" + std::to_string(readHumidity()) + "%";
    data += ", Light=" + std::to_string(readLightLevel());
    beam.notify(data);
    log_heartbeat("Auto-sensor data sent");
    lastSend = now;
  }
  
  delay(10);
}

