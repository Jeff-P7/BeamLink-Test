#include <Arduino.h>
#include "BeamLink.h"
#include "BeamLog.hpp"
#include "beam.config.h"
#include "NexState.h"

using namespace nexstate;

BeamLink beam;

// Button handling
static unsigned long lastButtonPress = 0;
static const unsigned long BUTTON_DEBOUNCE_MS = 200;

// BLE notification function
bool notifyBleClient(const std::string& message) {
    return beam.notify(message);
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(300); // Give USB CDC time to initialize

    LOG_INFO("BeamLink LED Toggle Example with NexState booting...");

    // Initialize NexState system
    NexStateConfig config;
    config.enableSerialOutput = true;
    config.enableJsonFormat = true;
    config.enableChangeDetection = true;
    config.outputOnChange = true;
    config.outputOnInterval = false; // Only output on changes
    config.outputIntervalMs = 1000;
    
    // Set device information (not state, but output context)
    config.deviceInfo.deviceName = DEVICE_NAME;
    config.deviceInfo.deviceId = DEVICE_ID;
    config.deviceInfo.deviceType = DEVICE_TYPE;
    config.deviceInfo.firmwareVersion = FIRMWARE_VERSION;
    config.deviceInfo.ledPin = LED_PIN;
    config.deviceInfo.ledActiveHigh = LED_ACTIVE_HIGH;

    if (!initialize(config)) {
        LOG_ERR("NexState initialization failed");
        return;
    }

    LOG_OK("NexState system initialized");

    // Set initial state (only dynamic values, not device info)
    State().set("ledOn", true); // Start with LED ON
    State().set("ledBlinking", false);
    State().set("bleConnected", false);

    // Subscribe to state changes
    State().subscribe([](const std::string& key, const std::string& value) {
        LOG_INFO("State changed: %s = %s", key.c_str(), value.c_str());
    });

    // Print initial configuration
    LOG_CFG("Config: name=%s id=%s type=%s fw=%s", DEVICE_NAME, DEVICE_ID, DEVICE_TYPE, FIRMWARE_VERSION);
    LOG_BLE("Service UUID: %s", BLE_SERVICE_UUID);
    LOG_BLE("Char UUID: %s", BLE_CHARACTERISTIC_UUID);

    // Initialize BLE
    const bool bleOk = beam.begin(
        BLE_NAME,
        BLE_POWER_DBM,
        BLE_ADV_INTERVAL_MS,
        BLE_SERVICE_UUID,
        BLE_CHARACTERISTIC_UUID
    );

    if (!bleOk) {
        LOG_ERR("BeamLink begin() failed — BLE not started");
        return;
    }

    // BLE notification function is handled by BeamLink directly

    // Ensure service UUID is in advertising
    auto* adv = NimBLEDevice::getAdvertising();
    adv->addServiceUUID(BLE_SERVICE_UUID);
    adv->setScanResponse(true);
    adv->start();

    LOG_BLE("Advertising as %s", BLE_NAME);

    // Set up message handler
    beam.onMessage([](const std::string& message, ReplyFn reply) {
        LOG_BLE("RX: %s", message.c_str());

        if (message == "led:on") {
            State().set("ledOn", true);
            State().set("ledBlinking", false);
            reply("LED ON");
            LOG_OK("LED turned ON via BLE");
        }
        else if (message == "led:off") {
            State().set("ledOn", false);
            State().set("ledBlinking", false);
            reply("LED OFF");
            LOG_OK("LED turned OFF via BLE");
        }
        else if (message == "led:status") {
            bool ledOn = State().get<bool>("ledOn", false);
            const char* stateStr = ledOn ? "ON" : "OFF";
            reply(std::string("LED ") + stateStr);
            LOG_INFO("LED status requested: %s", stateStr);
        }
        else if (message == "led:toggle") {
            bool currentLedOn = State().get<bool>("ledOn", false);
            State().set("ledOn", !currentLedOn);
            State().set("ledBlinking", false);
            const char* stateStr = !currentLedOn ? "ON" : "OFF";
            reply(std::string("LED ") + stateStr);
            LOG_OK("LED toggled to: %s via BLE", stateStr);
        }
        else if (message == "led:blink") {
            State().set("ledBlinking", true);
            State().set("ledOn", true);
            reply("LED BLINKING");
            LOG_OK("LED set to BLINKING mode via BLE");
        }
        else if (message == "state:info") {
            bool ledOn = State().get<bool>("ledOn", false);
            bool ledBlinking = State().get<bool>("ledBlinking", false);
            std::string stateInfo = std::string("State: ") + (ledOn ? "ON" : "OFF") +
                                   ", Blinking: " + (ledBlinking ? "YES" : "NO");
            reply(stateInfo);
            LOG_INFO("State info requested");
        }
        else if (message == "info") {
            bool ledOn = State().get<bool>("ledOn", false);
            std::string info = std::string("Device: ") + DEVICE_NAME +
                               ", ID: " + DEVICE_ID +
                               ", Type: " + DEVICE_TYPE +
                               ", FW: " + FIRMWARE_VERSION +
                               ", State: " + (ledOn ? "ON" : "OFF");
            reply(info);
            LOG_INFO("Info sent with state");
        }
        else {
            reply("Unknown Command");
            LOG_WARN("Unknown command: %s", message.c_str());
        }
    });

    // Perform boot blink sequence
    LOG_INFO("Starting boot blink sequence...");
    // Simple boot blink sequence
    pinMode(LED_PIN, OUTPUT);
    for (int i = 0; i < 2; i++) {
        digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? HIGH : LOW);
        delay(150);
        digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? LOW : HIGH);
        delay(150);
    }
    digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? HIGH : LOW); // Final state ON
    LOG_OK("Boot blink sequence completed (LED ON)");

    LOG_OK("Ready. Commands: led:on, led:off, led:status, led:toggle, led:blink, info");
}

void loop() {
    beam.loop();
    
    // Update NexState system (handles change detection and output)
    update();

    // Handle BOOT button for LED toggle (simplified - no g_pins)
    // Note: This is a placeholder - implement actual button handling as needed
    static unsigned long lastButtonCheck = 0;
    if (millis() - lastButtonCheck >= 100) { // Check every 100ms
        // Add actual button handling here if needed
        lastButtonCheck = millis();
    }

    // Update BLE connection state
    bool bleConnected = beam.isConnected();
    State().set("bleConnected", bleConnected);

    // Handle LED blinking mode
    bool ledBlinking = State().get<bool>("ledBlinking", false);
    if (ledBlinking) {
        static unsigned long lastBlinkTime = 0;
        unsigned long now = millis();
        if (now - lastBlinkTime >= 500) { // 500ms blink interval
            bool currentLedOn = State().get<bool>("ledOn", false);
            State().set("ledOn", !currentLedOn);
            lastBlinkTime = now;
        }
    }

    // Update hardware LED based on state
    bool ledOn = State().get<bool>("ledOn", false);
    digitalWrite(LED_PIN, LED_ACTIVE_HIGH ? (ledOn ? HIGH : LOW) : (ledOn ? LOW : HIGH));

    delay(10);
}