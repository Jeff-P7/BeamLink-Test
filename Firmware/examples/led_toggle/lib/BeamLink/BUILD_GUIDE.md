# BeamLink Build Guide

## Quick Start

### 1. Build the Main Library Test
```bash
cd BeamLink-ESP32
pio run
```

### 2. Build an Example
```bash
cd examples/led_toggle
pio run
```

## Project Structure

```
BeamLink-ESP32/
├── include/           # Library header files (.h)
│   ├── BeamLink.h
│   ├── BeamConfig.h
│   ├── BeamUtils.h
│   └── BeamErrors.h
├── src/              # Library implementation (.cpp)
│   ├── BeamLink.cpp
│   ├── BeamConfig.cpp
│   ├── BeamUtils.cpp
│   └── main.cpp      # Simple test/demo
├── examples/         # Example projects
│   ├── led_toggle/
│   └── sensor_monitor/
└── library.json      # Library metadata
```

## How It Works

1. **Library Files**: `include/` and `src/` contain the BeamLink library code
2. **Examples**: Each example has its own `platformio.ini` that points back to the library using `lib_extra_dirs = ../../`
3. **C++17**: All projects use C++17 for modern lambda support

## Building Examples

Examples automatically link to the BeamLink library code via `lib_extra_dirs = ../../`.

**From VS Code:**
- Open an example's `platformio.ini` file
- Click the "Build" button at the bottom (builds that specific example)

**From Command Line:**
```bash
cd examples/led_toggle
pio run           # Build
pio run -t upload # Upload to ESP32
pio device monitor # View serial output
```

## Key Changes Made

### Fixed Compilation Issues
✅ Added C++17 support (`-std=gnu++17`)
✅ Changed `auto reply` to `ReplyFn reply` in all lambdas
✅ Fixed `#include <Arduino.h>` in BeamErrors.h
✅ Fixed NimBLE API calls (init, setPower, notify)
✅ Changed `pServer` from unique_ptr to raw pointer

### Simplified Configuration
✅ Clean, minimal `platformio.ini` files
✅ Removed unnecessary build flags
✅ Clear comments explaining each setting

## Common Commands

```bash
# Build
pio run

# Upload
pio run -t upload

# Clean build
pio run -t clean

# Monitor serial
pio device monitor

# Full rebuild
pio run -t clean && pio run
```

## Troubleshooting

**Build fails with "auto" error**
→ Make sure you're using `ReplyFn reply` not `auto reply` in lambdas

**Examples can't find BeamLink**
→ Check that `lib_extra_dirs = ../../` is in the example's platformio.ini

**Wrong project building**
→ Open the `platformio.ini` file for the project you want to build

