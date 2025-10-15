# BeamLink Firmware Tools 🚀

Stylish build tools for BeamLink firmware development with emoji-rich output and colored logs.

## Quick Start

```bash
# Install dependencies
npm install

# Show available commands
npm run prettify:banner

# Full deployment pipeline
npm run flash:all

# Individual steps
npm run build
npm run flash:fw
npm run monitor
```

## Available Commands

| Command | Description | Emoji |
|---------|-------------|-------|
| `npm run build` | Build firmware | 🔨 |
| `npm run flash:fw` | Upload firmware | 💾 |
| `npm run flash:fs` | Upload filesystem | 🗂️ |
| `npm run flash:all` | Full deployment | 🚀 |
| `npm run monitor` | Serial monitor | 📺 |
| `npm run test` | Run tests | 🧪 |
| `npm run clean` | Clean build | 🧹 |

## Environment Variables

Override defaults as needed:

```bash
PIO_ENV=esp32dev PIO_EXAMPLE=examples/sensor_monitor npm run flash:all
```

## Logger.h Usage

Include `Logger.h` in your firmware for colored serial output:

```cpp
#include "Logger.h"

void setup() {
  log_info("Starting BeamLink...");
  log_config("Device: BeamLink-LED");
  log_ble("BLE initialized");
  log_success("Ready!");
}
```

## Features

- ⚡ Emoji-rich build output
- 🎨 Colored terminal logs
- 📡 Styled serial monitor logs
- 🚀 One-command deployment
- ⚙️ Configurable environments
