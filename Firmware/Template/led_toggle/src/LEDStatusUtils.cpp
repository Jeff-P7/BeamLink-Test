#include "LEDStatusUtils.h"
#include "LEDUtils.h"

namespace LEDStatusUtils {

void printLEDDeviceStatus(const char* deviceName, const char* deviceId,
                         const char* deviceType, const char* firmwareVersion,
                         bool connected, const char* serviceUuid, const char* characteristicUuid,
                         int ledPin, bool ledActiveHigh, uint32_t counter,
                         bool ledState, bool blinkingMode) {
  Serial.println();
  Serial.println("────────────────────────────────────────────");
  Serial.printf("🛰️  BeamLink Status [%lu]\n", counter);
  Serial.printf("Device Name      : %s\n", deviceName);
  Serial.printf("Device ID        : %s\n", deviceId);
  Serial.printf("BLE State        : %s\n", connected ? "CONNECTED ✅" : "IDLE / ADVERTISING");
  Serial.printf("Advertising      : %s\n", connected ? "NO" : "YES");
  Serial.printf("Service UUID     : %s\n", serviceUuid);
  Serial.printf("Characteristic   : %s\n", characteristicUuid);
  Serial.printf("LED Pin          : GPIO%d (%s)\n", ledPin, ledActiveHigh ? "ACTIVE HIGH" : "ACTIVE LOW");
  Serial.printf("LED State        : %s %s\n", ledState ? "ON" : "OFF", ledState ? "💡" : "⚫");
  Serial.printf("Blinking Mode    : %s %s\n", blinkingMode ? "YES" : "NO", blinkingMode ? "🔄" : "⏸️");
  Serial.printf("Free Heap        : %u bytes\n", ESP.getFreeHeap());
  Serial.printf("Uptime           : %.2f seconds\n", millis() / 1000.0);
  Serial.println("────────────────────────────────────────────");
}


} // namespace LEDStatusUtils
