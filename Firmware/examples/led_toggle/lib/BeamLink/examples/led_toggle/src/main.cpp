#include <Arduino.h>
#include "BeamLink.h"
#include "BeamLog.hpp"
#include "beam.config.h"

BeamLink beam;

static unsigned long lastStatus = 0;

static void waitForSerial(unsigned long ms = 300) {
  delay(ms); // gives USB CDC a moment so early prints aren't dropped
}

static void bootBlinkSequence() {
  LOG_INFO("Starting boot blink sequence...");
  // Start ON
  digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? HIGH : LOW);
  delay(150);

  // Blink a couple of times
  for (int i = 0; i < 2; i++) {
    // OFF
    digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? LOW : HIGH);
    delay(150);
    // ON
    digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? HIGH : LOW);
    delay(150);
  }

  // Ensure final state is ON
  digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? HIGH : LOW);
  LOG_OK("Boot blink sequence completed (LED ON)");
}

static bool ledIsOn() {
  // "On" means the pin currently driven to the active level
  return digitalRead(LED_PIN) == (LED_ACTIVE_HIGH ? HIGH : LOW);
}

static void printStatus() {
  static uint32_t counter = 0;
  counter++;

  // Check connection status - if not connected, we're advertising
  bool connected = beam.isConnected();
  bool advertising = !connected;  // If not connected, we're advertising

  Serial.println();
  Serial.println("────────────────────────────────────────────");
  Serial.printf("🛰️  BeamLink Status [%lu]\n", counter);
  Serial.printf("Device Name      : %s\n", DEVICE_NAME);
  Serial.printf("Device ID        : %s\n", DEVICE_ID);
  Serial.printf("BLE State        : %s\n", connected ? "CONNECTED ✅" : "IDLE / ADVERTISING");
  Serial.printf("Advertising      : %s\n", advertising ? "YES" : "NO");
  Serial.printf("Service UUID     : %s\n", BLE_SERVICE_UUID);
  Serial.printf("Characteristic   : %s\n", BLE_CHARACTERISTIC_UUID);
  Serial.printf("LED Pin          : GPIO%d (%s)\n", LED_PIN, LED_ACTIVE_HIGH ? "ACTIVE HIGH" : "ACTIVE LOW");
  Serial.printf("LED State        : %s\n", ledIsOn() ? "ON 💡" : "OFF ⚫");
  Serial.printf("Free Heap        : %u bytes\n", ESP.getFreeHeap());
  Serial.printf("Uptime           : %.2f seconds\n", millis() / 1000.0);
  Serial.println("────────────────────────────────────────────");
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  waitForSerial();

  LOG_INFO("BeamLink LED Toggle Example booting…");

  // Print config & UUIDs BEFORE begin() so you always see them at boot
  LOG_CFG("Config: name=%s id=%s type=%s fw=%s", DEVICE_NAME, DEVICE_ID, DEVICE_TYPE, FIRMWARE_VERSION);
  LOG_BLE("Service UUID (cfg): %s", BLE_SERVICE_UUID);
  LOG_BLE("Char    UUID (cfg): %s", BLE_CHARACTERISTIC_UUID);

  // LED init
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? HIGH : LOW);  // default ON
  LOG_PIN("GPIO%d set as OUTPUT (default ON)", LED_PIN);
  
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

  // Message handler (unchanged)
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
      reply(ledIsOn() ? "LED ON" : "LED OFF");
      LOG_INFO("LED status: %s", ledIsOn() ? "ON" : "OFF");
    }
    else if (in == "led:toggle") {
      bool on = ledIsOn();
      digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? !on : on);
      reply(on ? "LED OFF" : "LED ON");
      LOG_OK("LED toggled to: %s", on ? "OFF" : "ON");
    }
    else if (in == "info") {
      std::string info = std::string("Device: ") + DEVICE_NAME +
                         ", ID: " + DEVICE_ID +
                         ", Type: " + DEVICE_TYPE +
                         ", FW: " + FIRMWARE_VERSION;
      reply(info);
      LOG_INFO("Info sent");
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

  // Heartbeat every 1 second
  if (millis() - lastStatus >= 1000) {
    printStatus();
    lastStatus = millis();
  }

  delay(10);
}

