#include "BeamConfig.h"

/**
 * @brief Load configuration from compile-time constants
 * 
 * Since we've moved to header-based configuration, this function
 * now simply sets the configuration to default values. The actual
 * configuration should be done using the beam.config.h header file
 * in the application code.
 * 
 * @param cfg Reference to BeamConfig structure to populate
 * @param path Path parameter (ignored, kept for compatibility)
 * @return true (always succeeds since we use defaults)
 */
bool loadBeamConfig(BeamConfig& cfg, const char* path) {
  // Configuration is now handled via compile-time constants
  // This function is kept for compatibility but doesn't load from file
  
  Serial.println("Using default configuration (JSON loading removed)");
  Serial.printf("  Device: %s (%s)\n", cfg.deviceName.c_str(), cfg.deviceId.c_str());
  Serial.printf("  Type: %s, FW: %s\n", cfg.deviceType.c_str(), cfg.fwVersion.c_str());
  Serial.printf("  BLE: %s, Power: %d dBm, Interval: %d ms\n", 
                cfg.bleEnabled ? "enabled" : "disabled", cfg.blePowerDbm, cfg.bleAdvIntervalMs);
  Serial.printf("  Service UUID: %s\n", cfg.bleServiceUuid.c_str());
  
  return true;
}