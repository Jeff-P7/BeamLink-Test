#pragma once
#include <Arduino.h>
#include <string>

/**
 * @file BeamConfig.h
 * @brief Configuration structure and loader for BeamLink
 * 
 * This file defines the BeamConfig structure that holds all configuration
 * parameters for the BeamLink library, and provides a function to load
 * configuration from a file on SPIFFS.
 */

/**
 * @struct BeamConfig
 * @brief Configuration structure for BeamLink library
 * 
 * Contains all configurable parameters for the BeamLink BLE communication
 * library, including device information, BLE settings, and hardware pins.
 */
struct BeamConfig {
  // Identity
  std::string deviceId   = "BLX-01A2B3";        ///< Unique device identifier
  std::string deviceName = "BeamLink-ESP32";    ///< Device name
  std::string deviceType = "BLE-Controller";    ///< Device type description
  std::string fwVersion  = "1.0.0";             ///< Firmware version string

  // BLE Configuration
  bool bleEnabled         = true;               ///< Enable BLE
  std::string bleName     = "BeamLink-ESP32";   ///< BLE advertising name (max 32 chars)
  int blePowerDbm         = 9;                  ///< Advertising power in dBm (-12 to +9)
  int bleAdvIntervalMs    = 100;                ///< Advertising interval in ms (20-10240)
  std::string bleServiceUuid       = "12345678-1234-1234-1234-1234567890ab"; ///< BLE Service UUID
  std::string bleCharacteristicUuid = "12345678-1234-1234-1234-1234567890ac"; ///< BLE Characteristic UUID

  // Wi-Fi Configuration
  bool wifiEnabled        = false;              ///< Enable Wi-Fi
  std::string wifiSsid    = "";                 ///< Wi-Fi SSID
  std::string wifiPass    = "";                 ///< Wi-Fi password
  std::string wifiMode    = "STA";              ///< Wi-Fi mode (STA/AP/APSTA)

  // Cloud / OTA
  bool cloudEnabled       = false;              ///< Enable cloud connectivity
  std::string cloudEndpoint = "https://api.beamlink.io"; ///< Cloud API endpoint
  bool otaEnabled         = true;               ///< Enable OTA updates
  std::string otaUrl      = "https://firmware.beamlink.io/esp32/latest.bin"; ///< OTA firmware URL

  // Hardware Mapping
  int ledPin              = 2;                  ///< LED pin number (0-39)
  bool ledActiveHigh      = true;               ///< LED active high (true) or low (false)
  std::string sensorPins  = "34,35";            ///< Sensor pin mapping (comma-separated)
  std::string actuatorPins = "12,13,14";        ///< Actuator pin mapping (comma-separated)

  // Behavior
  int reportIntervalMs    = 5000;               ///< Report interval in ms
  bool autoReconnect      = true;               ///< Auto-reconnect on disconnect
  std::string logLevel    = "INFO";             ///< Log level (DEBUG/INFO/WARN/ERROR)
  int serialBaud          = 115200;             ///< Serial baud rate (9600-2000000)
  bool debugMode          = true;               ///< Enable debug output

  // Security
  std::string authToken   = "";                 ///< Authentication token
  bool encryption         = false;              ///< Enable encryption

  // Calibration
  float sensorGain        = 1.0;                ///< Sensor gain multiplier
  float zeroOffset        = 0.02;               ///< Zero offset calibration
};

/**
 * @brief Load configuration from file
 * 
 * Loads configuration parameters from a text file on SPIFFS.
 * The file should contain KEY=VALUE pairs, one per line.
 * Comments start with # and empty lines are ignored.
 * 
 * @param cfg Reference to BeamConfig structure to populate
 * @param path Path to configuration file (default: "/beam.config")
 * @return true if configuration was loaded successfully, false otherwise
 * 
 * @note If the file doesn't exist or fails to load, the config structure
 *       retains its default values.
 * 
 * @example
 * ```cpp
 * BeamConfig config;
 * if (loadBeamConfig(config)) {
 *   Serial.println("Configuration loaded successfully");
 * } else {
 *   Serial.println("Using default configuration");
 * }
 * ```
 */
bool loadBeamConfig(BeamConfig& cfg, const char* path = "/beam.config");
