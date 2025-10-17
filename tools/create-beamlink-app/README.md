# Create-BeamLink-App

A CLI tool to scaffold complete BeamLink IoT projects with ESP32 firmware and React Native mobile app.

## 🚀 Installation

```bash
npm install -g create-beamlink-app
```

## 📱 Usage

```bash
create-beamlink-app my-iot-project
```

This will create a complete project structure with:

```
my-iot-project/
├── firmware/          # ESP32 firmware template
│   ├── src/main.cpp
│   ├── include/beam.config.h
│   ├── platformio.ini
│   └── README.md
└── app/               # React Native mobile app template
    ├── components/
    ├── hooks/
    ├── types/
    ├── constants/
    ├── src/
    ├── App.tsx
    ├── package.json
    └── README.md
```

## ⚙️ Options

The CLI will prompt you for:

- **Project name**: Name for your project directory
- **Include firmware**: Whether to include ESP32 firmware template
- **Include mobile app**: Whether to include React Native app template
- **Device type**: ESP32 variant (DevKitC, S2, S3, C3, or Custom)

## 🔧 Next Steps

After creating your project:

### ESP32 Firmware
```bash
cd my-iot-project/firmware
pio run --target uploadfs  # Upload config
pio run --target upload     # Upload firmware
pio device monitor          # View output
```

### Mobile App
```bash
cd my-iot-project/app
npm install
npx expo run:android  # or npx expo run:ios
```

## 📚 Features

- **Interactive Setup**: Guided project creation with prompts
- **Template Selection**: Choose which components to include
- **Device Configuration**: Automatic ESP32 board configuration
- **Complete Examples**: Working LED control and sensor monitoring
- **Documentation**: Generated README with setup instructions

## 🛠️ Development

To develop the CLI tool locally:

```bash
cd tools/create-beamlink-app
npm install
npm link
```

## 📄 License

MIT License - See main BeamLink project for details.
