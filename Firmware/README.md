# BeamLink Firmware

This directory contains the BeamLink ESP32 BLE library and example projects.

## 📁 Structure

```
Firmware/
├── BeamLink-ESP32/          # The BeamLink library
│   ├── include/             # Library header files
│   ├── src/                 # Library implementation
│   ├── test/               # Unit tests
│   └── README.md           # Library documentation
│
└── examples/               # Example projects using BeamLink
    ├── led_toggle/         # Simple LED control example
    └── sensor_monitor/     # Advanced sensor monitoring example
```

## 🚀 Quick Start

### Using the Examples

The `examples/` directory contains complete projects that demonstrate how to use BeamLink in your own applications.

#### LED Toggle Example
```bash
cd examples/led_toggle
pio run --target uploadfs    # Upload config file
pio run --target upload       # Upload firmware
pio device monitor            # View serial output
```

#### Sensor Monitor Example
```bash
cd examples/sensor_monitor
pio run --target uploadfs    # Upload config file
pio run --target upload       # Upload firmware
pio device monitor            # View serial output
```

### Using BeamLink in Your Project

1. **Reference the library** in your `platformio.ini`:
   ```ini
   [env:esp32dev]
   platform = espressif32
   board = esp32dev
   framework = arduino
   
   build_unflags = -std=gnu++11
   build_flags = -std=gnu++17
   
   lib_extra_dirs = ../path/to/Firmware/
   
   lib_deps = 
       h2zero/NimBLE-Arduino @ ^1.4.3
   ```

2. **Include BeamLink** in your code:
   ```cpp
   #include "BeamLink.h"
   #include "BeamConfig.h"
   
   BeamLink beam;
   
   void setup() {
     beam.begin("MyDevice");
     beam.onMessage([](const std::string& msg, ReplyFn reply) {
       reply("Echo: " + msg);
     });
   }
   
   void loop() {
     beam.loop();
     delay(10);
   }
   ```

## 📚 Documentation

- **Library Documentation**: See [BeamLink-ESP32/README.md](BeamLink-ESP32/README.md)
- **LED Example**: See [examples/led_toggle/README.md](examples/led_toggle/README.md)
- **Sensor Example**: See [examples/sensor_monitor/README.md](examples/sensor_monitor/README.md)

## 🎯 What's What

### BeamLink-ESP32
The core library that provides BLE communication functionality. This is what you'll reference in your projects via `lib_extra_dirs`.

**Key Files:**
- `BeamLink.h/cpp` - Main BLE communication class
- `BeamConfig.h/cpp` - Configuration file parsing
- `BeamUtils.h/cpp` - Utility functions
- `BeamErrors.h` - Error handling framework

### Examples
Ready-to-use projects that demonstrate BeamLink integration. These are **not** part of the library—they're complete applications that **use** the library.

**LED Toggle** - Basic example showing:
- Simple BLE messaging
- Configuration file usage
- GPIO control

**Sensor Monitor** - Advanced example showing:
- Statistics tracking
- Message parsing utilities
- Auto-reporting
- Multiple command formats

## 🔧 Development

### Building the Library
The library itself doesn't need to be built separately. It's compiled as part of your project when you reference it via `lib_extra_dirs`.

### Running Tests
```bash
cd BeamLink-ESP32
pio test
```

### Library Structure
The library follows PlatformIO's library format:
- `library.json` - Library metadata
- `include/` - Public headers
- `src/` - Implementation files
- `examples/` - (Deprecated - use root examples instead)

## 📄 License

MIT License - See individual LICENSE files for details.

