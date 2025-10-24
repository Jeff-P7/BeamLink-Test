#pragma once

#include <Arduino.h>
#include <string>

/**
 * @brief Status display utilities for BeamLink LED examples
 * 
 * Provides common status display functions for LED control examples.
 * This is specific to LED examples and not part of the core BeamLink library.
 */

namespace LEDStatusUtils {

/**
 * @brief Print LED device status
 * @param deviceName device name
 * @param deviceId device ID
 * @param deviceType device type
 * @param firmwareVersion firmware version
 * @param connected BLE connection status
 * @param serviceUuid BLE service UUID
 * @param characteristicUuid BLE characteristic UUID
 * @param ledPin LED pin number
 * @param ledActiveHigh LED active high setting
 * @param counter status counter
 * @param ledState current LED state
 * @param blinkingMode whether LED is blinking
 */
void printLEDDeviceStatus(const char* deviceName, const char* deviceId,
                          const char* deviceType, const char* firmwareVersion,
                          bool connected, const char* serviceUuid, const char* characteristicUuid,
                          int ledPin, bool ledActiveHigh, uint32_t counter,
                          bool ledState, bool blinkingMode);

} // namespace LEDStatusUtils
