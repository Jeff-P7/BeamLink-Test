#include "LEDMessageHandler.h"
#include "BeamLog.hpp"

namespace LEDMessageHandler {

// LED Command Handler Implementation
LEDCommandHandler::LEDCommandHandler(int pin, bool activeHigh, const char* name, const char* id, 
                                   const char* type, const char* fw)
  : ledPin(pin), ledActiveHigh(activeHigh), deviceName(name), deviceId(id), 
    deviceType(type), firmwareVersion(fw), ledState(false), blinkingMode(false), lastBlinkTime(0) {
  
  LOG_INFO("LEDCommandHandler initialized");
}

void LEDCommandHandler::handleMessage(const std::string& message, std::function<void(const std::string&)> reply) {
  LOG_BLE("RX: %s", message.c_str());

  if (message == "led:on") {
    ledState = true;
    blinkingMode = false;
    LEDUtils::turnOn(ledPin, ledActiveHigh);
    reply("LED ON");
    LOG_OK("LED turned ON via BLE");
  }
  else if (message == "led:off") {
    ledState = false;
    blinkingMode = false;
    LEDUtils::turnOff(ledPin, ledActiveHigh);
    reply("LED OFF");
    LOG_OK("LED turned OFF via BLE");
  }
  else if (message == "led:status") {
    const char* stateStr = ledState ? "ON" : "OFF";
    reply(std::string("LED ") + stateStr);
    LOG_INFO("LED status requested: %s", stateStr);
  }
  else if (message == "led:toggle") {
    ledState = !ledState;
    blinkingMode = false;
    if (ledState) {
      LEDUtils::turnOn(ledPin, ledActiveHigh);
    } else {
      LEDUtils::turnOff(ledPin, ledActiveHigh);
    }
    const char* stateStr = ledState ? "ON" : "OFF";
    reply(std::string("LED ") + stateStr);
    LOG_OK("LED toggled to: %s via BLE", stateStr);
  }
  else if (message == "led:blink") {
    blinkingMode = true;
    ledState = true; // Start blinking from ON state
    reply("LED BLINKING");
    LOG_OK("LED set to BLINKING mode via BLE");
  }
  else if (message == "state:info") {
    std::string stateInfo = std::string("State: ") + (ledState ? "ON" : "OFF") +
                           ", Blinking: " + (blinkingMode ? "YES" : "NO");
    reply(stateInfo);
    LOG_INFO("State info requested");
  }
  else if (message == "info") {
    std::string info = std::string("Device: ") + deviceName +
                       ", ID: " + deviceId +
                       ", Type: " + deviceType +
                       ", FW: " + firmwareVersion +
                       ", State: " + (ledState ? "ON" : "OFF");
    reply(info);
    LOG_INFO("Info sent with state");
  }
  else {
    reply("Unknown Command");
    LOG_WARN("Unknown command: %s", message.c_str());
  }
}

void LEDCommandHandler::update() {
  // Handle blinking mode
  if (blinkingMode) {
    if (millis() - lastBlinkTime >= 500) { // 500ms blink interval
      ledState = !ledState;
      if (ledState) {
        LEDUtils::turnOn(ledPin, ledActiveHigh);
      } else {
        LEDUtils::turnOff(ledPin, ledActiveHigh);
      }
      lastBlinkTime = millis();
    }
  }
}

bool LEDCommandHandler::refreshFromSerial(const std::string& serialInput) {
  if (serialInput == "on" || serialInput == "1") {
    ledState = true;
    blinkingMode = false;
    LEDUtils::turnOn(ledPin, ledActiveHigh);
    LOG_INFO("LED turned ON via serial");
    return true;
  }
  else if (serialInput == "off" || serialInput == "0") {
    ledState = false;
    blinkingMode = false;
    LEDUtils::turnOff(ledPin, ledActiveHigh);
    LOG_INFO("LED turned OFF via serial");
    return true;
  }
  else if (serialInput == "blink" || serialInput == "toggle") {
    blinkingMode = true;
    ledState = true;
    LOG_INFO("LED set to BLINKING mode via serial");
    return true;
  }
  else if (serialInput == "status") {
    const char* stateStr = ledState ? "ON" : "OFF";
    LOG_INFO("LED status: %s (blinking: %s)", stateStr, blinkingMode ? "YES" : "NO");
    return false; // No state change, just query
  }
  
  return false;
}

} // namespace LEDMessageHandler
