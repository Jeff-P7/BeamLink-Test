# BeamLink-ESP32 Quick Start Guide

This guide will help you get your BeamLink firmware running in minutes.

## What Was Created

```
BeamLink-ESP32/
├── include/           # Core library headers
│   ├── BeamLink.h    # Main beam API
│   ├── BeamConfig.h  # Configuration system
│   └── Uuids.h       # BLE UUIDs
├── src/              # Core library implementation
│   ├── BeamLink.cpp
│   ├── BeamConfig.cpp
│   └── main.cpp      # Simple echo example
└── examples/
    └── led_toggle/   # LED control example ⭐
        ├── src/main.cpp
        ├── include/beam.config.h
        └── platformio.ini
```

## Option 1: Test the Echo Example (Simplest)

This is already in the main project:

```bash
cd Firmware/BeamLink-ESP32
pio run --target upload
pio device monitor
```

**Expected output:**
```
BeamLink Echo Test
Initializing BeamLink BLE...
BeamLink ready, advertising as: BeamLink-Test
```

**Connect with nRF Connect:**
- Scan for "BeamLink-Test"
- Write to RX: `hello`
- Receive from TX: `Echo: hello`

## Option 2: LED Toggle Example (Full Featured)

Complete example with SPIFFS config and LED control:

```bash
cd Firmware/BeamLink-ESP32/examples/led_toggle

# 1. Upload config file to SPIFFS
pio run --target uploadfs

# 2. Upload firmware
pio run --target upload

# 3. Monitor serial output
pio device monitor
```

**Expected output:**
```
BeamLink LED Toggle Example
Loading config from /beam.config
Config loaded:
  Device: BeamLink-ESP32 (BLK-LED-001)
  Type: BLE-Controller, FW: 1.0.0
LED initialized on pin 2
BeamLink ready, advertising as: BeamLink-ESP32
Commands: led:on, led:off, led:status
```

**BLE Commands:**
- `led:on` → LED turns ON, replies "LED ON"
- `led:off` → LED turns OFF, replies "LED OFF"
- `led:status` → Get current state

## Testing with Mobile Apps

### nRF Connect (Recommended)

**Android:** https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp
**iOS:** https://apps.apple.com/app/nrf-connect-for-mobile/id1054362403

1. Open nRF Connect
2. Tap **SCAN**
3. Find "BeamLink-ESP32"
4. Tap **CONNECT**
5. Expand the service (UUID: 1234...890ab)
6. Find RX characteristic (UUID ending in ...ac)
7. Tap **↑ (upload icon)** to write
8. Select **TEXT**
9. Type: `led:on`
10. Send!

### LightBlue (iOS)

1. Scan and connect to "BeamLink-ESP32"
2. Tap the service
3. Tap RX characteristic (Write)
4. Enter: `led:on`
5. Tap "Write"

### Web Bluetooth (Chrome)

You can also test from Chrome:
```javascript
// Coming soon - Web Bluetooth example
```

## Customizing the LED Example

Edit `examples/led_toggle/include/beam.config.h`:

```ini
# Change device name
DEVICE_NAME = MyDevice

# Use different GPIO pin
LED_PIN = 2

# Inverted LED (if built-in LED is active-low)
LED_ACTIVE_HIGH = false
```

**After editing:**
```bash
pio run --target uploadfs  # Re-upload config
pio run --target upload     # Re-upload firmware
```

## Creating Your Own Project

### 1. Copy the LED Example

```bash
cp -r examples/led_toggle my_project
cd my_project
```

### 2. Modify src/main.cpp

```cpp
#include <Arduino.h>
#include "BeamLink.h"

void setup() {
  Serial.begin(115200);
  
  beam.begin("MyDevice");
  
  beam.onMessage([](const std::string& in, auto reply) {
    // Your custom logic here
    if (in == "hello") {
      reply("world");
    }
  });
}

void loop() {
  beam.loop();
  delay(50);
}
```

### 3. Update platformio.ini

```ini
lib_extra_dirs = ../../  # Path to BeamLink-ESP32 library
```

### 4. Build and Upload

```bash
pio run --target upload
```

## Common Issues

### ❌ "fatal error: BeamLink.h: No such file or directory"

**Fix:** Check `lib_extra_dirs` in platformio.ini points to BeamLink-ESP32

```ini
lib_extra_dirs = ../../BeamLink-ESP32
```

### ❌ LED doesn't respond

**Fix:** Check your LED pin and polarity in beam.config:
```ini
LED_PIN = 2              # GPIO 2 for most ESP32 boards
LED_ACTIVE_HIGH = true   # Try false if LED seems inverted
```

### ❌ "Config file /beam.config not found"

**Fix:** Upload SPIFFS filesystem first:
```bash
pio run --target uploadfs
```

### ❌ Can't connect from phone

**Solutions:**
- Power cycle ESP32
- Try another BLE app
- Check serial monitor for connection logs
- Ensure only one BLE example is running

## Next Steps

1. ✅ **Sensors**: Add sensor readings to BLE responses
2. ✅ **Actuators**: Control motors, servos, or relays
3. ✅ **Multi-device**: Connect multiple ESP32s
4. ✅ **Mobile App**: Build a React Native app (see App/BeamLink-Expo)

## Need Help?

- 📖 Main README: `README.md`
- 📖 LED Example: `examples/led_toggle/README.md`
- 📖 API Reference: Check header files in `include/`

## API Cheat Sheet

```cpp
// Initialize BLE
beam.begin("DeviceName");

// Handle incoming messages
beam.onMessage([](const std::string& in, auto reply) {
  reply("Response message");
});

// Send notification
beam.notify("Status update");

// Call in loop
beam.loop();
```

---

**Happy Building! 🚀**
