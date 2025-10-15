#ifndef BEAM_CONFIG_H
#define BEAM_CONFIG_H

// Device Identity Configuration
#define DEVICE_ID "BLX-LED-001"
#define DEVICE_NAME "BeamLink-LED"
#define DEVICE_TYPE "BLE-Controller"
#define FIRMWARE_VERSION "1.0.0"

// BLE Configuration
#define BLE_ENABLED true
#define BLE_NAME "BeamLink-LED"
#define BLE_POWER_DBM 9
#define BLE_ADV_INTERVAL_MS 100
#define BLE_SERVICE_UUID "12345678-1234-1234-1234-1234567890ab"
#define BLE_RX_UUID "12345678-1234-1234-1234-1234567890ac"
#define BLE_TX_UUID "12345678-1234-1234-1234-1234567890ad"
#define BLE_OTA_SERVICE_UUID "12345678-0000-0000-0000-1234567890aa"
#define BLE_SENSOR_SERVICE_UUID "12345678-0000-0000-0000-1234567890af"

// WiFi Configuration
#define WIFI_ENABLED false
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define WIFI_MODE "STA"

// Cloud Configuration
#define CLOUD_ENABLED false
#define CLOUD_ENDPOINT "https://api.beamlink.io"

// OTA Configuration
#define OTA_ENABLED true
#define OTA_URL "https://firmware.beamlink.io/esp32/latest.bin"

// Hardware Configuration
#define LED_PIN 2
#define LED_ACTIVE_HIGH true
#define SENSOR_PINS "34,35"
#define ACTUATOR_PINS "12,13,14"

// Behavior Configuration
#define REPORT_INTERVAL_MS 5000
#define AUTO_RECONNECT true
#define LOG_LEVEL "INFO"
#define SERIAL_BAUD 115200
#define DEBUG_MODE true

// Security Configuration
#define AUTH_TOKEN ""
#define ENCRYPTION_ENABLED false

// Calibration Configuration
#define SENSOR_GAIN 1.0f
#define ZERO_OFFSET 0.02f

#endif // BEAM_CONFIG_H
