# Sensor Monitor Example

This example demonstrates advanced BeamLink features including:

- **Statistics tracking** - Monitor messages sent/received, errors, and uptime
- **MTU negotiation** - Support for larger messages (up to 512 bytes)
- **Message parsing utilities** - Parse command:action and key=value formats
- **Auto-reporting** - Periodic sensor data notifications
- **Comprehensive command handling** - Multiple command formats supported

## Hardware Requirements

- ESP32 development board
- (Optional) Real sensors: DHT22, BH1750, etc.

## Quick Start

1. **Upload filesystem:**
   ```bash
   cd examples/sensor_monitor
   pio run --target uploadfs
   ```

2. **Upload firmware:**
   ```bash
   pio run --target upload
   ```

3. **Monitor output:**
   ```bash
   pio device monitor
   ```

## Available Commands

### Sensor Readings
- `temp` - Get temperature reading
- `humidity` - Get humidity reading  
- `light` - Get light level reading
- `all` - Get all sensor readings at once

### System Information
- `stats` - Show statistics (messages, errors, uptime)
- `uptime` - Show system uptime
- `mtu` - Show current MTU size
- `info` - Show device information
- `help` - Show available commands

### Configuration
- `config:name` - Get device name
- `config:id` - Get device ID
- `config:fw` - Get firmware version

### Formatted Queries
- `get:temp` - Get just the temperature value
- `get:hum` - Get just the humidity value
- `get:light` - Get just the light value

### Management
- `reset` - Reset statistics counters

## Auto-Reporting

When connected, the device automatically sends sensor readings every 30 seconds.

## Example Session

```
> help
< Commands: temp, humidity, light, stats, all, uptime, reset, help

> temp
< Temperature: 25.3°C

> humidity
< Humidity: 48.7%

> all
< Temp=25.3°C, Hum=48.7%, Light=512

> stats
< Stats: RX=4, TX=4, Errors=0, Uptime=1m 23s

> config:name
< Device: BeamLink-Sensor

> mtu
< MTU: 512 bytes
```

## Features Demonstrated

### 1. Statistics Tracking
```cpp
beam.getMessagesReceived();
beam.getMessagesSent();
beam.getErrors();
beam.getUptime();
beam.resetStats();
```

### 2. MTU Information
```cpp
beam.getMTU();  // Returns negotiated MTU size
```

### 3. Message Parsing
```cpp
using namespace BeamUtils;

// Parse "command:action"
std::string cmd, action;
if (parseCommand(msg, cmd, action)) {
  // Handle structured command
}

// Parse "key1=val1,key2=val2"
auto params = parseKeyValue(msg);
```

### 4. Utility Functions
```cpp
BeamUtils::formatStats(rx, tx, errors, uptime);
BeamUtils::formatUptime(milliseconds);
BeamUtils::trim(str);
BeamUtils::toLower(str);
```

## Customization

### Add Real Sensors

Replace the simulated sensor functions with real sensor libraries:

```cpp
#include <DHT.h>
DHT dht(4, DHT22);

float readTemperature() {
  return dht.readTemperature();
}

float readHumidity() {
  return dht.readHumidity();
}
```

### Change Auto-Report Interval

Modify the interval in `loop()`:

```cpp
if (beam.isConnected() && (now - lastSend > 60000)) {  // 60 seconds
  // Send data
  lastSend = now;
}
```

### Disable Auto-Reporting

Comment out the auto-send code in `loop()`:

```cpp
// Optional: Auto-send sensor readings every 30 seconds
// static unsigned long lastSend = 0;
// ...
```

## Troubleshooting

### No sensor data
- Check that BLE client is connected
- Verify commands are sent correctly
- Check serial monitor for errors

### Large messages truncated
- Check MTU size with `mtu` command
- Messages are auto-truncated to fit MTU
- Default MTU is 23 bytes, negotiated up to 512 bytes

### Statistics not updating
- Ensure `beam.loop()` is called regularly
- Check that messages are being sent/received
- Use `reset` command to clear statistics

## Next Steps

1. Add real sensor hardware
2. Implement data logging
3. Add sensor thresholds and alerts
4. Create mobile app for data visualization
5. Add BLE security (pairing/bonding)

