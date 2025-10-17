#include <Arduino.h>
#include "BeamLink.h"
#include "BeamLog.hpp"
#include "BeamUtils.h"
#include "beam.config.h"

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

BeamLink beam;

static unsigned long lastStatus = 0;

static void waitForSerial(unsigned long ms = 300) {
  delay(ms); // gives USB CDC a moment so early prints aren't dropped
}

static void bootBlinkSequence() {
  LOG_INFO("Starting boot blink sequence...");
  pinMode(LED_PIN, OUTPUT);
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? HIGH : LOW);
    delay(200);
    digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? LOW : HIGH);
    delay(200);
  }
  LOG_OK("Boot blink sequence completed");
}

static void printStatus() {
  static uint32_t counter = 0;
  counter++;

  // Check connection status - if not connected, we're advertising
  bool connected = beam.isConnected();
  bool advertising = !connected;  // If not connected, we're advertising

  Serial.println();
  Serial.println("────────────────────────────────────────────");
  Serial.printf("📊 BeamLink Sensor Monitor Status [%lu]\n", counter);
  Serial.printf("Device Name      : %s\n", DEVICE_NAME);
  Serial.printf("Device ID        : %s\n", DEVICE_ID);
  Serial.printf("BLE State        : %s\n", connected ? "CONNECTED ✅" : "IDLE / ADVERTISING");
  Serial.printf("Advertising      : %s\n", advertising ? "YES" : "NO");
  Serial.printf("Service UUID     : %s\n", BLE_SERVICE_UUID);
  Serial.printf("Characteristic   : %s\n", BLE_CHARACTERISTIC_UUID);
  Serial.printf("Temperature      : %.1f°C\n", readTemperature());
  Serial.printf("Humidity         : %.1f%%\n", readHumidity());
  Serial.printf("Light Level      : %d/1023\n", readLightLevel());
  Serial.printf("Free Heap        : %u bytes\n", ESP.getFreeHeap());
  Serial.printf("Uptime           : %.2f seconds\n", millis() / 1000.0);
  Serial.println("────────────────────────────────────────────");
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  waitForSerial();

  LOG_INFO("BeamLink Sensor Monitor Example booting…");

  // Print config & UUIDs BEFORE begin() so you always see them at boot
  LOG_CFG("Config: name=%s id=%s type=%s fw=%s", DEVICE_NAME, DEVICE_ID, DEVICE_TYPE, FIRMWARE_VERSION);
  LOG_BLE("Service UUID (cfg): %s", BLE_SERVICE_UUID);
  LOG_BLE("Char    UUID (cfg): %s", BLE_CHARACTERISTIC_UUID);

  // Boot blink sequence
  bootBlinkSequence();

  // Start BLE
  const bool ok = beam.begin(
    BLE_NAME,
    BLE_POWER_DBM,
    BLE_ADV_INTERVAL_MS,
    BLE_SERVICE_UUID,
    BLE_CHARACTERISTIC_UUID
  );

  if (!ok) {
    LOG_ERR("BeamLink begin() failed — BLE not started");
    return;
  }

  // Ensure our service UUID is present in advertising
  // If your BeamLink already handles this internally, this is harmless.
  auto* adv = NimBLEDevice::getAdvertising();
  adv->addServiceUUID(BLE_SERVICE_UUID);
  adv->setScanResponse(true);
  adv->start();

  LOG_BLE("Advertising as %s", BLE_NAME);
  LOG_BLE("Service UUID (active): %s", BLE_SERVICE_UUID);
  LOG_BLE("Char    UUID (active): %s", BLE_CHARACTERISTIC_UUID);

  // Set up comprehensive message handler
  beam.onMessage([](const std::string& msg, ReplyFn reply) {
    using namespace BeamUtils;
    
    LOG_BLE("RX: %s", msg.c_str());
    
    std::string cmd, action, value;
    
    // Handle simple commands
    if (msg == "help") {
      reply("Commands: temp, humidity, light, stats, all, uptime, reset, help");
      LOG_INFO("Help command sent");
    }
    else if (msg == "temp") {
      float temp = readTemperature();
      reply("Temperature: " + std::to_string(temp) + "°C");
      LOG_INFO("Temperature reading: %.1f°C", temp);
    }
    else if (msg == "humidity") {
      float hum = readHumidity();
      reply("Humidity: " + std::to_string(hum) + "%");
      LOG_INFO("Humidity reading: %.1f%%", hum);
    }
    else if (msg == "light") {
      int light = readLightLevel();
      reply("Light: " + std::to_string(light) + "/1023");
      LOG_INFO("Light reading: %d/1023", light);
    }
    else if (msg == "all") {
      // Send all sensor readings
      float temp = readTemperature();
      float hum = readHumidity();
      int light = readLightLevel();
      std::string data = "Temp=" + std::to_string(temp) + "°C";
      data += ", Hum=" + std::to_string(hum) + "%";
      data += ", Light=" + std::to_string(light);
      reply(data);
      LOG_INFO("All sensors: T=%.1f°C H=%.1f%% L=%d", temp, hum, light);
    }
    else if (msg == "stats") {
      std::string stats = formatStats(
        beam.getMessagesReceived(),
        beam.getMessagesSent(),
        beam.getErrors(),
        beam.getUptime()
      );
      reply(stats);
      LOG_INFO("Statistics sent");
    }
    else if (msg == "uptime") {
      reply("Uptime: " + formatUptime(beam.getUptime()));
      LOG_INFO("Uptime requested");
    }
    else if (msg == "reset") {
      beam.resetStats();
      reply("Statistics reset");
      LOG_OK("Statistics reset");
    }
    else if (msg == "mtu") {
      reply("MTU: " + std::to_string(beam.getMTU()) + " bytes");
      LOG_INFO("MTU: %d bytes", beam.getMTU());
    }
    else if (msg == "info") {
      std::string info = std::string("Device: ") + DEVICE_NAME +
                         ", ID: " + DEVICE_ID +
                         ", Type: " + DEVICE_TYPE +
                         ", FW: " + FIRMWARE_VERSION;
      reply(info);
      LOG_INFO("Info sent");
    }
    // Handle command:action format
    else if (parseCommand(msg, cmd, action)) {
      if (cmd == "config") {
        if (action == "name") {
          reply("Device: " + std::string(DEVICE_NAME));
          LOG_INFO("Config name requested");
        } else if (action == "id") {
          reply("ID: " + std::string(DEVICE_ID));
          LOG_INFO("Config ID requested");
        } else if (action == "fw") {
          reply("Firmware: " + std::string(FIRMWARE_VERSION));
          LOG_INFO("Config firmware requested");
        } else {
          reply("Unknown config: " + action);
          LOG_WARN("Unknown config action: %s", action.c_str());
        }
      }
      else if (cmd == "get") {
        if (action == "temp") {
          float temp = readTemperature();
          reply(std::to_string(temp));
          LOG_INFO("Get temp: %.1f°C", temp);
        } else if (action == "hum") {
          float hum = readHumidity();
          reply(std::to_string(hum));
          LOG_INFO("Get humidity: %.1f%%", hum);
        } else if (action == "light") {
          int light = readLightLevel();
          reply(std::to_string(light));
          LOG_INFO("Get light: %d", light);
        } else {
          reply("Unknown sensor: " + action);
          LOG_WARN("Unknown sensor: %s", action.c_str());
        }
      }
      else {
        reply("Unknown command: " + cmd);
        LOG_WARN("Unknown command: %s", cmd.c_str());
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
      LOG_INFO("Parsed %d key=value parameters", params.size());
    }
    else {
      reply("Unknown command. Type 'help' for commands.");
      LOG_WARN("Unknown command: %s", msg.c_str());
    }
  });
  
  LOG_OK("Ready. Commands: temp, humidity, light, all, stats, uptime, reset, info, help");
}

void loop() {
  beam.loop();

  // Heartbeat every 5 seconds
  if (millis() - lastStatus >= 5000) {
    printStatus();
    lastStatus = millis();
  }
  
  // Optional: Auto-send sensor readings every 30 seconds
  static unsigned long lastSend = 0;
  unsigned long now = millis();
  
  if (beam.isConnected() && (now - lastSend > 30000)) {
    float temp = readTemperature();
    float hum = readHumidity();
    int light = readLightLevel();
    std::string data = "Auto: Temp=" + std::to_string(temp) + "°C";
    data += ", Hum=" + std::to_string(hum) + "%";
    data += ", Light=" + std::to_string(light);
    beam.notify(data);
    LOG_INFO("Auto-sent sensor data: T=%.1f°C H=%.1f%% L=%d", temp, hum, light);
    lastSend = now;
  }
  
  delay(10);
}

