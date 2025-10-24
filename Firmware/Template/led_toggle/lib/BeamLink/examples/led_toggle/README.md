# BeamLink LED Toggle Example

A complete example demonstrating BLE-controlled LED using the BeamLink framework for ESP32.

## Features

- ✅ BLE communication using NimBLE
- ✅ Configuration file support (SPIFFS)
- ✅ Simple command-based LED control
- ✅ Real-time status reporting
- ✅ Auto-reconnect on disconnect

## Hardware Requirements

- ESP32 development board (ESP32-DevKitC, WROOM, or similar)
- Built-in LED on GPIO 2 (or configure your own)

## Getting Started

### 1. Clone or Copy Example

```bash
cd Firmware/BeamLink-ESP32/examples/led_toggle
```

### 2. Upload Filesystem (SPIFFS)

The `beam.config` file needs to be uploaded to the ESP32's SPIFFS filesystem:

```bash
pio run --target uploadfs
```

### 3. Build and Upload Firmware

```bash
pio run --target upload
pio device monitor
```

## Configuration

Edit `include/beam.config.h` to customize your device:

```ini
DEVICE_NAME = BeamLink-ESP32
DEVICE_ID = BLK-LED-001
LED_PIN = 2
LED_ACTIVE_HIGH = true
```

## BLE Commands

Connect using any BLE terminal app (nRF Connect, LightBlue, etc.):

| Command | Description | Response |
|---------|-------------|----------|
| `led:on` | Turn LED on | `LED ON` |
| `led:off` | Turn LED off | `LED OFF` |
| `led:status` | Get LED status | `LED ON` or `LED OFF` |

## BLE Service UUIDs

- **Service:** `12345678-1234-1234-1234-1234567890ab`
- **RX (Write):** `12345678-1234-1234-1234-1234567890ac`
- **TX (Notify):** `12345678-1234-1234-1234-1234567890ad`

## Testing with nRF Connect

1. Open nRF Connect app
2. Scan for "BeamLink-ESP32"
3. Connect to the device
4. Find the RX characteristic (ends in ...ac)
5. Send text commands: `led:on`, `led:off`, `led:status`
6. Read responses from TX characteristic (ends in ...ad)

## Code Structure

```
led_toggle/
├── src/
│   └── main.cpp          # Main application
├── include/
│   └── beam.config.h     # Device configuration
├── platformio.ini        # Build configuration
└── README.md            # This file
```

## How It Works

```cpp
// Initialize BLE with config
beam.begin(config.deviceName.c_str());

// Handle incoming commands
beam.onMessage([&](const std::string& in, auto reply) {
  if (in == "led:on") {
    digitalWrite(LED_PIN, HIGH);
    reply("LED ON");
  }
});
```

## Troubleshooting

### LED doesn't respond
- Check LED_PIN in beam.config matches your hardware
- Verify LED_ACTIVE_HIGH setting (some boards are inverted)

### Can't upload filesystem
```bash
# Ensure SPIFFS is enabled in platformio.ini
board_build.filesystem = spiffs
```

### Device not advertising
- Check serial monitor for initialization messages
- Verify NimBLE-Arduino library is installed
- Try power cycling the ESP32

## Next Steps

- Modify commands to control other peripherals
- Add sensor data reporting
- Implement custom protocols
- Build a mobile app companion

## License

MIT License - See main BeamLink repository for details
