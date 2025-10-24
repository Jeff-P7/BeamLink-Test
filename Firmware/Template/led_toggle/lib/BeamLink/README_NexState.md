# NexState - ESP32 State Management System

NexState is a Zustand-like state management system for ESP32/C++ that provides efficient state tracking with change detection to avoid unnecessary serial output loops.

## Key Features

- **Change Detection**: Only outputs state when values actually change
- **No Polling Loops**: Eliminates constant `Serial.print()` calls every second
- **Memory Efficient**: Tracks changes, not constant polling
- **JSON Support**: Output state in JSON format for easy parsing
- **Type Safety**: Template-based typed state values
- **Subscription**: Callbacks for state change events
- **Configurable**: Flexible output and detection options

## Problem Solved

Traditional ESP32 firmware often includes loops like this:
```cpp
void loop() {
    Serial.println("LED: " + String(ledOn ? "ON" : "OFF"));
    Serial.println("Connected: " + String(bleConnected ? "YES" : "NO"));
    delay(1000); // Output every second regardless of changes
}
```

This creates:
- Constant serial spam
- Unnecessary CPU usage
- Hard-to-read logs
- Poor performance

## NexState Solution

```cpp
void loop() {
    // Only outputs when state actually changes
    update(); // Handles change detection and output
    
    // State changes trigger automatic output
    State().set("ledOn", newLedState);
    State().set("bleConnected", newConnectionState);
}
```

## Usage

### Basic Setup

```cpp
#include "NexState.h"

void setup() {
    // Initialize NexState
    NexStateConfig config;
    config.enableSerialOutput = true;
    config.enableJsonFormat = true;
    config.enableChangeDetection = true;
    config.outputOnChange = true;
    
    // Set device information (not state, but output context)
    config.deviceInfo.deviceName = "MyDevice";
    config.deviceInfo.deviceId = "device-001";
    config.deviceInfo.deviceType = "sensor";
    config.deviceInfo.firmwareVersion = "1.0.0";
    
    initialize(config);
    
    // Set initial state (only dynamic values)
    State().set("ledOn", false);
    State().set("connected", false);
}

void loop() {
    update(); // Call this in your main loop
    
    // State changes automatically trigger output with device info
    State().set("ledOn", digitalRead(LED_PIN) == HIGH);
}
```

### State Management

```cpp
// Set state values (only dynamic values, not device info)
State().set("ledOn", true);
State().set("temperature", 25.5f);
State().set("motionDetected", false);

// Get state values
bool ledOn = State().get<bool>("ledOn", false);
float temp = State().get<float>("temperature", 0.0f);
bool motion = State().get<bool>("motionDetected", false);

// Check for changes
if (State().hasChanged("ledOn")) {
    // LED state changed
}

// Get all changed keys
auto changed = State().getChangedKeys();
for (const auto& key : changed) {
    Serial.println("Changed: " + key);
}
```

### Subscription to Changes

```cpp
// Subscribe to state changes
State().subscribe([](const std::string& key, const std::string& value) {
    Serial.printf("State changed: %s = %s\n", key.c_str(), value.c_str());
});
```

### Configuration Options

```cpp
NexStateConfig config;
config.enableSerialOutput = true;     // Enable serial output
config.enableJsonFormat = true;       // Output in JSON format
config.enableChangeDetection = true;  // Enable change detection
config.outputOnChange = true;          // Output immediately on change
config.outputOnInterval = false;       // Output periodically
config.outputIntervalMs = 1000;        // Interval for periodic output
```

## Output Formats

### JSON Format (default)
```json
{"device":"MyDevice","id":"device-001","type":"sensor","fw":"1.0.0","state":{"ledOn":true,"temperature":25.5,"connected":false}}
```

### Text Format
```
Device: MyDevice (ID: device-001, Type: sensor, FW: 1.0.0) | State: ledOn=true, temperature=25.5, connected=false
```

## Examples

### LED Controller
```cpp
void loop() {
    update();
    
    // Handle button press
    if (digitalRead(BUTTON_PIN) == LOW) {
        bool currentLed = State().get<bool>("ledOn", false);
        State().set("ledOn", !currentLed);
        // Output happens automatically due to change detection
    }
    
    // Update hardware based on state
    bool ledOn = State().get<bool>("ledOn", false);
    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
}
```

### Sensor Monitor
```cpp
void loop() {
    update();
    
    // Read sensor
    float temperature = readTemperature();
    bool motion = readMotionSensor();
    
    // Only update if values changed
    State().set("temperature", temperature);
    State().set("motion", motion);
    
    // State output only occurs when values actually change
}
```

## Benefits

1. **Performance**: No unnecessary serial output
2. **Debugging**: Clear visibility of state changes
3. **Memory Efficient**: Only tracks changes, not constant polling
4. **Reactive**: Immediate output when state changes
5. **Clean Logs**: No spam, only meaningful changes
6. **Type Safe**: Compile-time type checking
7. **Flexible**: Configurable output formats and timing

## Integration with BeamLink

NexState works seamlessly with the BeamLink BLE communication library:

```cpp
beam.onMessage([](const std::string& message, ReplyFn reply) {
    if (message == "led:on") {
        State().set("ledOn", true);
        reply("LED ON");
    }
    else if (message == "led:status") {
        bool ledOn = State().get<bool>("ledOn", false);
        reply(ledOn ? "LED ON" : "LED OFF");
    }
});
```

## File Structure

```
Firmware/BeamLink-ESP32/
├── include/
│   └── NexState.h          # Main header file
├── src/
│   └── NexState.cpp        # Implementation
└── examples/
    └── nexstate_led_toggle/ # Example usage
        ├── src/main.cpp
        ├── platformio.ini
        └── README.md
```

## Requirements

- ESP32 platform
- Arduino framework
- C++17 support
- PlatformIO (recommended) or Arduino IDE

## License

Part of the BeamLink project. See main project license for details.
