# BeamLink-Expo

A modern React Native application for discovering, connecting to, and controlling ESP32 devices via Bluetooth Low Energy (BLE). Built with Expo and TypeScript for seamless ESP32 communication.

## ✨ Features

- 🔍 **Advanced BLE Scanning**: Discover nearby Bluetooth devices with real-time updates
- 🎯 **ESP32 Detection**: Automatically identify and filter ESP32 devices by manufacturer ID and name patterns
- 📱 **Modern UI**: Clean, intuitive interface with Material Design principles
- 🔧 **TypeScript**: Fully typed for better development experience and reliability
- 🛡️ **Error Handling**: Comprehensive error handling with user-friendly feedback
- 📊 **Device Information**: Detailed device information including RSSI, services, and manufacturer data
- 🎨 **Customizable**: Easy to customize colors, spacing, and configuration
- 🔗 **Device Connection**: Connect to ESP32 devices and control LEDs remotely
- 💡 **LED Control**: Turn on/off, toggle, and check status of ESP32 LEDs
- 📱 **Real-time Communication**: Send commands and receive responses from ESP32 devices
- 🔄 **Connection Management**: Automatic reconnection and connection state monitoring
- 📋 **Device List Management**: Freeze/unfreeze device list, sorting, and filtering

## 🎯 ESP32 Device Support

The app automatically detects various ESP32 variants:
- ESP32 (standard)
- ESP32-S2
- ESP32-S3
- ESP32-C3
- Custom ESP32 devices with BeamLink firmware

## 📋 Requirements

- React Native with Expo
- Development build (BLE functionality requires native modules)
- Android/iOS device with Bluetooth Low Energy support
- Proper permissions for Bluetooth and location access
- ESP32 device running BeamLink firmware

## 🚀 Installation

1. **Clone the repository:**
```bash
git clone https://github.com/yourusername/BeamLink.git
cd BeamLink/App/BeamLink-Expo
```

2. **Install dependencies:**
```bash
npm install
# or
yarn install
```

3. **Create a development build:**
```bash
npx expo install expo-dev-client
npx expo run:android
# or
npx expo run:ios
```

## 📱 Usage

1. **Start the app** on your device
2. **Grant permissions** when prompted for Bluetooth and location access
3. **Tap "Start Scan"** to begin discovering nearby devices
4. **View discovered devices** with detailed information including RSSI and services
5. **Filter ESP32 devices** using the toggle button to show only ESP32 devices
6. **Connect to devices** by tapping on ESP32 devices
7. **Control LEDs** using the LED control buttons (on/off/toggle/status)
8. **Monitor connection status** and device responses in real-time

## 🏗️ Project Structure

```
├── components/          # Reusable UI components
│   ├── DeviceCard.tsx   # Device display and control component
│   ├── ScanControls.tsx # Scan control buttons and filters
│   ├── ErrorBanner.tsx  # Error display component
│   └── LEDControl.tsx  # LED control interface
├── hooks/               # Custom React hooks
│   └── useBLE.ts        # BLE functionality and state management
├── types/               # TypeScript type definitions
│   └── ble.ts           # BLE-related types and interfaces
├── constants/           # App constants and configuration
│   └── ble.ts           # BLE configuration constants
├── src/                 # Additional source files
│   ├── components/      # Development components
│   │   └── DevOverlay.tsx # Development overlay
│   └── utils/           # Utility functions
│       ├── logger.ts    # Logging utilities
│       └── notify.ts    # Notification utilities
├── App.tsx              # Main application component
└── package.json         # Dependencies and scripts
```

## ⚙️ Configuration

Key configuration options in `constants/ble.ts`:

### BLE Configuration
- `SCAN_TIMEOUT`: How long to scan for devices (default: 10 seconds)
- `RSSI_THRESHOLD`: Minimum signal strength to display (-100 dBm)
- `MAX_DEVICES`: Maximum number of devices to store (50)
- `RETRY_ATTEMPTS`: Number of connection retry attempts (3)

### ESP32 Detection
- `MANUFACTURER_IDS`: Espressif manufacturer IDs for device detection
- `COMMON_SERVICE_UUIDS`: Known ESP32 service UUIDs
- `NAME_PATTERNS`: Common ESP32 device name patterns
- `SERVICE_UUID`: BeamLink service UUID for communication
- `CHARACTERISTIC_UUID`: BeamLink characteristic UUID for data exchange

### LED Commands
- `LED_COMMANDS`: Available LED control commands (on/off/toggle/status)
- `LED_RESPONSES`: Expected responses from ESP32 devices

## 🔧 Development

The app is built with modern technologies:

- **React Native** with Expo for cross-platform development
- **TypeScript** for type safety and better development experience
- **react-native-ble-plx** for robust BLE functionality
- **Custom hooks** for clean separation of concerns and state management
- **Styled components** for maintainable and responsive UI
- **Toast notifications** for user feedback
- **Comprehensive logging** for debugging and monitoring

### Key Features Implementation

- **Device Scanning**: Real-time BLE device discovery with filtering
- **ESP32 Detection**: Automatic identification using manufacturer data and name patterns
- **Connection Management**: Robust connection handling with retry logic
- **LED Control**: Complete LED control interface with status monitoring
- **Error Handling**: Comprehensive error management with user-friendly messages
- **State Management**: Clean state management using custom hooks

## 🎯 ESP32 Integration

The app is designed to work seamlessly with ESP32 devices running BeamLink firmware:

### Supported Commands
- `led:on` - Turn LED on
- `led:off` - Turn LED off
- `led:toggle` - Toggle LED state
- `led:status` - Get current LED status
- `info` - Get device information
- `ping` - Test connectivity

### Communication Protocol
- Uses custom BLE service UUID: `12345678-1234-1234-1234-1234567890ab`
- Uses custom characteristic UUID: `12345678-1234-1234-1234-1234567890ac`
- Message-based communication with request/response pattern
- Real-time notifications for status updates

## 🚀 Future Enhancements

- [ ] **Advanced Device Control**: Support for more ESP32 peripherals (sensors, actuators)
- [ ] **Data Logging**: Store and visualize sensor data from ESP32 devices
- [ ] **Device Pairing**: Secure pairing and bonding with ESP32 devices
- [ ] **Connection History**: Save and manage favorite devices
- [ ] **Advanced Filtering**: More sophisticated device filtering options
- [ ] **Dark Mode**: Complete dark theme support
- [ ] **Multi-device Support**: Connect to multiple ESP32 devices simultaneously
- [ ] **Custom Commands**: User-defined command interface
- [ ] **Firmware Updates**: OTA firmware update capability
- [ ] **Data Export**: Export device data and logs

## 🐛 Troubleshooting

### Common Issues

**Device not appearing in scan:**
- Ensure ESP32 is running BeamLink firmware
- Check if Bluetooth is enabled on your device
- Verify ESP32 is advertising (check serial monitor)
- Try refreshing the scan

**Connection fails:**
- Ensure ESP32 is not already connected to another device
- Check if the device is within range
- Try multiple connection attempts
- Restart both the app and ESP32

**LED commands not working:**
- Verify connection is established
- Check ESP32 serial monitor for received commands
- Ensure ESP32 firmware supports LED control
- Try reconnecting to the device

## 🤝 Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Follow the existing code style and TypeScript conventions
4. Add tests for new functionality
5. Update documentation
6. Submit a pull request

### Development Setup

```bash
git clone https://github.com/yourusername/BeamLink.git
cd BeamLink/App/BeamLink-Expo
npm install
npx expo start
```

## 📄 License

MIT License - see LICENSE file for details.

---

<div align="center">

**Built with ❤️ for the ESP32 community**

[⭐ Star this repo](https://github.com/yourusername/BeamLink) • [🐛 Report Bug](https://github.com/yourusername/BeamLink/issues) • [💡 Request Feature](https://github.com/yourusername/BeamLink/issues)

</div>
