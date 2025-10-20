# BeamLink-ESP32 Features Guide

Complete guide to all BeamLink features, from basic to advanced usage.

## Table of Contents

1. [Basic Usage](#basic-usage)
2. [Configuration](#configuration)
3. [Statistics Tracking](#statistics-tracking)
4. [Message Parsing Utilities](#message-parsing-utilities)
5. [Error Handling](#error-handling)
6. [Advanced Features](#advanced-features)
7. [Best Practices](#best-practices)

## Basic Usage

### Minimal Setup

```cpp
#include <Arduino.h>
#include "BeamLink.h"

BeamLink beam;

void setup() {
  Serial.begin(115200);
  
  // Initialize BLE (defaults: power=9dBm, interval=100ms)
  if (!beam.begin("MyDevice")) {
    Serial.println("Failed to initialize!");
    return;
  }
  
  // Handle messages
  beam.onMessage([](const std::string& msg, auto reply) {
    reply("Echo: " + msg);
  });
}

void loop() {
  beam.loop();
  delay(10);
}
```

### With Configuration

```cpp
#include <Arduino.h>
#include <SPIFFS.h>
#include "BeamLink.h"
#include "BeamConfig.h"

BeamConfig config;
BeamLink beam;

void setup() {
  Serial.begin(115200);
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS failed!");
    return;
  }
  
  // Initialize configuration (now uses compile-time constants)
  loadBeamConfig(config);
  
  // Initialize with config parameters
  if (!beam.begin(config.deviceName.c_str(), 
                  config.advPowerDbm, 
                  config.advIntervalMs)) {
    Serial.println("Failed to initialize!");
    return;
  }
  
  beam.onMessage([](const std::string& msg, auto reply) {
    reply("Got: " + msg);
  });
}

void loop() {
  beam.loop();
  delay(10);
}
```

## Configuration

### Config File Format

Create `include/beam.config.h`:

```ini
# Device Information
DEVICE_NAME = MyDevice
DEVICE_ID = DEV-001
DEVICE_TYPE = Sensor
FIRMWARE_VERSION = 1.0.0

# BLE Settings
BLE_ADVERTISING_POWER = 9          # -12 to +9 dBm
BLE_ADVERTISING_INTERVAL_MS = 100  # 20 to 10240 ms

# Hardware
LED_PIN = 2
LED_ACTIVE_HIGH = true

# Debug
SERIAL_BAUD = 115200
DEBUG_MODE = true
```

### Upload Configuration

```bash
pio run --target uploadfs  # Upload SPIFFS filesystem
pio run --target upload     # Upload firmware
```

### Configuration Parameters

| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| DEVICE_NAME | String | 1-32 chars | BeamLink-ESP32 | BLE device name |
| DEVICE_ID | String | 1-16 chars | BLK-001 | Unique identifier |
| BLE_ADVERTISING_POWER | Integer | -12 to +9 | 9 | TX power in dBm |
| BLE_ADVERTISING_INTERVAL_MS | Integer | 20 to 10240 | 100 | Advertising interval |
| LED_PIN | Integer | 0-39 | 2 | GPIO pin for LED |
| LED_ACTIVE_HIGH | Boolean | true/false | true | LED polarity |
| SERIAL_BAUD | Integer | 9600-2000000 | 115200 | Serial baud rate |
| DEBUG_MODE | Boolean | true/false | true | Enable debug output |

## Statistics Tracking

### Available Statistics

```cpp
uint32_t received = beam.getMessagesReceived();
uint32_t sent = beam.getMessagesSent();
uint32_t errors = beam.getErrors();
unsigned long uptime = beam.getUptime();  // milliseconds
uint16_t mtu = beam.getMTU();             // bytes
```

### Statistics Command Handler

```cpp
beam.onMessage([](const std::string& msg, auto reply) {
  if (msg == "stats") {
    std::string stats = BeamUtils::formatStats(
      beam.getMessagesReceived(),
      beam.getMessagesSent(),
      beam.getErrors(),
      beam.getUptime()
    );
    reply(stats);
    // Output: "Stats: RX=42, TX=38, Errors=2, Uptime=1h 23m 45s"
  }
  else if (msg == "uptime") {
    reply(BeamUtils::formatUptime(beam.getUptime()));
    // Output: "1h 23m 45s"
  }
  else if (msg == "reset") {
    beam.resetStats();
    reply("Statistics reset");
  }
});
```

### Auto-Monitoring

```cpp
void loop() {
  beam.loop();
  
  // Log statistics every 60 seconds
  static unsigned long lastLog = 0;
  if (millis() - lastLog > 60000) {
    Serial.printf("Stats - RX:%u TX:%u Errors:%u Uptime:%lums\n",
      beam.getMessagesReceived(),
      beam.getMessagesSent(),
      beam.getErrors(),
      beam.getUptime()
    );
    lastLog = millis();
  }
  
  delay(10);
}
```

## Message Parsing Utilities

### Command:Action Format

```cpp
#include "BeamUtils.h"

beam.onMessage([](const std::string& msg, auto reply) {
  std::string cmd, action;
  
  if (BeamUtils::parseCommand(msg, cmd, action)) {
    // Successfully parsed "command:action"
    
    if (cmd == "led") {
      if (action == "on") {
        digitalWrite(LED_PIN, HIGH);
        reply("LED ON");
      }
      else if (action == "off") {
        digitalWrite(LED_PIN, LOW);
        reply("LED OFF");
      }
    }
    else if (cmd == "sensor") {
      if (action == "read") {
        float value = readSensor();
        reply(std::to_string(value));
      }
    }
  }
  else {
    reply("Invalid format. Use command:action");
  }
});
```

### Command:Action:Value Format

```cpp
beam.onMessage([](const std::string& msg, auto reply) {
  std::string cmd, action, value;
  
  if (BeamUtils::parseCommandValue(msg, cmd, action, value)) {
    // Successfully parsed "command:action:value"
    
    if (cmd == "set" && action == "pin") {
      int pin = std::stoi(value);
      pinMode(pin, OUTPUT);
      reply("Pin " + value + " configured");
    }
    else if (cmd == "pwm" && action == "duty") {
      int duty = std::stoi(value);
      ledcWrite(0, duty);
      reply("PWM duty set to " + value);
    }
  }
});
```

### Key=Value Format

```cpp
beam.onMessage([](const std::string& msg, auto reply) {
  // Parse "key1=val1,key2=val2,key3=val3"
  auto params = BeamUtils::parseKeyValue(msg);
  
  if (params.empty()) {
    reply("No parameters found");
    return;
  }
  
  // Access parsed values
  if (params.count("pin")) {
    int pin = std::stoi(params["pin"]);
    Serial.printf("Pin: %d\n", pin);
  }
  
  if (params.count("state")) {
    std::string state = params["state"];
    Serial.printf("State: %s\n", state.c_str());
  }
  
  reply("Configured " + std::to_string(params.size()) + " parameters");
});
```

### String Utilities

```cpp
#include "BeamUtils.h"

// Trim whitespace
std::string trimmed = BeamUtils::trim("  hello  ");
// Result: "hello"

// Case conversion
std::string lower = BeamUtils::toLower("HELLO");
std::string upper = BeamUtils::toUpper("hello");

// String matching
bool starts = BeamUtils::startsWith("hello world", "hello");  // true
bool ends = BeamUtils::endsWith("hello world", "world");      // true

// Split strings
auto parts = BeamUtils::split("a,b,c", ',');
// Result: ["a", "b", "c"]
```

## Error Handling

### Using Error Codes

```cpp
#include "BeamErrors.h"

void handleError(BeamErrors::ErrorCode code) {
  if (BeamErrors::isOk(code)) {
    Serial.println("Success!");
    return;
  }
  
  // Get human-readable message
  Serial.printf("Error: %s\n", BeamErrors::toString(code));
  
  // Check error category
  if (BeamErrors::isInitError(code)) {
    Serial.println("Initialization failed, check configuration");
  }
  else if (BeamErrors::isConnectionError(code)) {
    Serial.println("Connection issue, check BLE client");
  }
  else if (BeamErrors::isMessageError(code)) {
    Serial.println("Message problem, check size and format");
  }
}
```

### Custom Error Handler

```cpp
class MyErrorHandler : public BeamErrors::ErrorHandler {
public:
  void onError(BeamErrors::ErrorCode code, const std::string& message) override {
    Serial.printf("[ERROR %d] %s: %s\n", 
      static_cast<int>(code),
      BeamErrors::toString(code),
      message.c_str()
    );
    
    // Take action based on error
    if (code == BeamErrors::ErrorCode::CONNECTION_LOST) {
      // Restart advertising
      Serial.println("Attempting to reconnect...");
    }
  }
};

MyErrorHandler errorHandler;
// Use errorHandler for custom error handling
```

## Advanced Features

### Large Message Support

```cpp
beam.onMessage([](const std::string& msg, auto reply) {
  // MTU is negotiated up to 512 bytes
  // Can send large JSON payloads
  
  std::string largeResponse = "{";
  largeResponse += "\"temperature\":25.3,";
  largeResponse += "\"humidity\":48.7,";
  largeResponse += "\"pressure\":1013.2,";
  largeResponse += "\"light\":512,";
  largeResponse += "\"motion\":false,";
  largeResponse += "\"battery\":87.5";
  largeResponse += "}";
  
  // Will automatically truncate if needed
  reply(largeResponse);
  
  // Check MTU
  Serial.printf("MTU: %d bytes\n", beam.getMTU());
});
```

### Connection Status Monitoring

```cpp
void loop() {
  static bool wasConnected = false;
  bool isConnected = beam.isConnected();
  
  // Detect connection changes
  if (isConnected && !wasConnected) {
    Serial.println("Client connected!");
    beam.notify("Welcome! Type 'help' for commands.");
  }
  else if (!isConnected && wasConnected) {
    Serial.println("Client disconnected!");
  }
  
  wasConnected = isConnected;
  
  beam.loop();
  delay(10);
}
```

### Auto-Reporting Sensor Data

```cpp
void loop() {
  beam.loop();
  
  // Send sensor data every 30 seconds when connected
  static unsigned long lastSend = 0;
  unsigned long now = millis();
  
  if (beam.isConnected() && (now - lastSend > 30000)) {
    std::string data = "Temp=" + std::to_string(readTemperature());
    data += ",Hum=" + std::to_string(readHumidity());
    
    if (beam.notify(data)) {
      Serial.println("Auto-report sent");
    }
    
    lastSend = now;
  }
  
  delay(10);
}
```

### Multi-Format Command Handler

```cpp
beam.onMessage([](const std::string& msg, auto reply) {
  using namespace BeamUtils;
  
  std::string cmd, action, value;
  
  // Try command:action:value format first
  if (parseCommandValue(msg, cmd, action, value)) {
    handleCommandValue(cmd, action, value, reply);
    return;
  }
  
  // Try command:action format
  if (parseCommand(msg, cmd, action)) {
    handleCommand(cmd, action, reply);
    return;
  }
  
  // Try key=value format
  if (msg.find('=') != std::string::npos) {
    auto params = parseKeyValue(msg);
    handleKeyValue(params, reply);
    return;
  }
  
  // Simple single-word commands
  handleSimpleCommand(msg, reply);
});
```

## Best Practices

### 1. Always Check begin() Result

```cpp
void setup() {
  if (!beam.begin("MyDevice", 9, 100)) {
    Serial.println("Failed to initialize BeamLink!");
    // Don't continue without successful initialization
    while(1) delay(1000);
  }
}
```

### 2. Use Configuration Files

```cpp
// Don't hardcode values
// beam.begin("MyDevice", 9, 100);  ❌

// Use configuration from compile-time constants
loadBeamConfig(config);
beam.begin(config.deviceName.c_str(), 
           config.advPowerDbm, 
           config.advIntervalMs);  ✅
```

### 3. Validate Message Sizes

```cpp
beam.onMessage([](const std::string& msg, auto reply) {
  // Check message size against MTU
  uint16_t maxSize = beam.getMTU() - 3;  // -3 for header
  
  std::string response = generateLargeResponse();
  
  if (response.length() > maxSize) {
    Serial.printf("Warning: Response truncated from %zu to %u bytes\n",
      response.length(), maxSize);
  }
  
  reply(response);
});
```

### 4. Monitor Statistics

```cpp
void loop() {
  beam.loop();
  
  // Periodic health check
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 300000) {  // Every 5 minutes
    if (beam.getErrors() > 100) {
      Serial.println("WARNING: High error count!");
      beam.resetStats();
    }
    lastCheck = millis();
  }
  
  delay(10);
}
```

### 5. Use Utilities for Parsing

```cpp
// Don't manually parse strings
// size_t pos = msg.find(':');  ❌
// std::string cmd = msg.substr(0, pos);

// Use built-in utilities
using namespace BeamUtils;
std::string cmd, action;
if (parseCommand(msg, cmd, action)) {  ✅
  // Handle parsed command
}
```

### 6. Handle Errors Gracefully

```cpp
beam.onMessage([](const std::string& msg, auto reply) {
  try {
    // Your message handling code
    processMessage(msg, reply);
  }
  catch (const std::exception& e) {
    Serial.printf("Error processing message: %s\n", e.what());
    reply("Error: " + std::string(e.what()));
  }
});
```

### 7. Provide Help Commands

```cpp
beam.onMessage([](const std::string& msg, auto reply) {
  if (msg == "help" || msg == "?") {
    reply("Commands: temp, humidity, light, stats, help");
    return;
  }
  
  // Other command handling...
});
```

### 8. Log Important Events

```cpp
beam.onMessage([](const std::string& msg, auto reply) {
  Serial.printf("RX [%u]: %s\n", beam.getMessagesReceived(), msg.c_str());
  
  // Process message...
  
  Serial.printf("TX [%u]: %s\n", beam.getMessagesSent(), response.c_str());
  reply(response);
});
```

## Performance Tips

### 1. Optimize Loop Delay

```cpp
void loop() {
  beam.loop();
  delay(10);  // Good balance of responsiveness and CPU usage
}
```

### 2. Batch Operations

```cpp
// Instead of multiple notify() calls
// beam.notify("Temp: 25.3");  ❌
// beam.notify("Hum: 48.7");   ❌

// Batch into one message
std::string data = "Temp:25.3,Hum:48.7,Light:512";  ✅
beam.notify(data);
```

### 3. Use Appropriate Power Levels

```cpp
// For long range: higher power (more battery)
beam.begin("LongRange", 9, 100);

// For power saving: lower power (less range)
beam.begin("PowerSaver", -12, 1000);

// Balanced (default)
beam.begin("Balanced", 0, 100);
```

## Troubleshooting

### Issue: Large messages not received

**Solution**: Check MTU size

```cpp
Serial.printf("MTU: %d bytes\n", beam.getMTU());
// Max effective payload is MTU - 3 bytes
```

### Issue: High error count

**Solution**: Monitor and debug

```cpp
if (beam.getErrors() > 0) {
  Serial.printf("Errors: %u\n", beam.getErrors());
  beam.resetStats();  // Reset after logging
}
```

### Issue: Commands not parsed correctly

**Solution**: Use built-in parsers

```cpp
using namespace BeamUtils;

// Debug parsing
std::string cmd, action;
if (parseCommand(msg, cmd, action)) {
  Serial.printf("Parsed: cmd='%s' action='%s'\n", 
    cmd.c_str(), action.c_str());
} else {
  Serial.printf("Failed to parse: '%s'\n", msg.c_str());
}
```

## Resources

- **Main Documentation**: [README.md](README.md)
- **Quick Start**: [QUICKSTART.md](QUICKSTART.md)
- **Improvements**: [IMPROVEMENTS.md](IMPROVEMENTS.md)
- **API Reference**: See header files in `include/`
- **Examples**: See `examples/` directory

## Support

For issues, questions, or contributions:
- Create an issue on GitHub
- Check existing documentation
- Review example code
- Run unit tests: `pio test`

---

**Happy Building with BeamLink! 🚀**

