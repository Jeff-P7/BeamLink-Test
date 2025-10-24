#pragma once

#include <Arduino.h>
#include <string>
#include <functional>
#include "BeamUtils.h"
#include "LEDUtils.h"

/**
 * @brief LED message handler for BeamLink LED examples
 * 
 * Provides LED-specific message handling for LED control examples.
 * This is specific to LED examples and not part of the core BeamLink library.
 */

namespace LEDMessageHandler {

/**
 * @brief LED command handler for LED control devices
 */
class LEDCommandHandler {
private:
  int ledPin;
  bool ledActiveHigh;
  const char* deviceName;
  const char* deviceId;
  const char* deviceType;
  const char* firmwareVersion;
  
  // Simple LED state tracking
  bool ledState;
  bool blinkingMode;
  unsigned long lastBlinkTime;

public:
  LEDCommandHandler(int pin, bool activeHigh, const char* name, const char* id, 
                   const char* type, const char* fw);
  
  void handleMessage(const std::string& message, std::function<void(const std::string&)> reply);
  
  // Update LED state (call from main loop)
  void update();
  
  // Refresh state from serial input
  bool refreshFromSerial(const std::string& serialInput);
  
  // Get current state info
  bool getCurrentState() const { return ledState; }
  bool isBlinking() const { return blinkingMode; }
};

} // namespace LEDMessageHandler
