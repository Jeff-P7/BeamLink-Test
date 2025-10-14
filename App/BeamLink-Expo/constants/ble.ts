export const BLE_CONFIG = {
  SCAN_TIMEOUT: 10000, // 10 seconds
  RSSI_THRESHOLD: -100, // Minimum RSSI to consider
  MAX_DEVICES: 50, // Maximum devices to store
  RETRY_ATTEMPTS: 3,
} as const;

export const ESP32_CONFIG = {
  MANUFACTURER_IDS: ['0x02E5', '0x00E0'], // Espressif manufacturer IDs
  COMMON_SERVICE_UUIDS: [
    '0000FFE0-0000-1000-8000-00805F9B34FB', // Common ESP32 service
    '0000180A-0000-1000-8000-00805F9B34FB', // Device Information
    '12345678-1234-1234-1234-123456789abc', // Our LED control service
  ],
  NAME_PATTERNS: ['ESP32', 'ESP32-', 'ESP_', 'NodeMCU', 'WROOM', 'WROVER', 'ESP32_LED_Controller'],
  LED_SERVICE_UUID: '12345678-1234-1234-1234-123456789abc',
  LED_CHARACTERISTIC_UUID: '87654321-4321-4321-4321-cba987654321',
  LED_COMMANDS: {
    ON: 'ON',
    OFF: 'OFF',
    TOGGLE: 'TOGGLE',
    STATUS: 'STATUS',
  },
  LED_RESPONSES: {
    LED_ON: 'LED_ON',
    LED_OFF: 'LED_OFF',
  },
} as const;

export const PERMISSIONS = {
  ANDROID: [
    'android.permission.BLUETOOTH',
    'android.permission.BLUETOOTH_ADMIN',
    'android.permission.BLUETOOTH_CONNECT',
    'android.permission.BLUETOOTH_SCAN',
    'android.permission.ACCESS_FINE_LOCATION',
    'android.permission.ACCESS_COARSE_LOCATION',
  ],
  IOS: [
    'BluetoothAlwaysUsageDescription',
    'BluetoothPeripheralUsageDescription',
  ],
} as const;

export const UI_CONFIG = {
  COLORS: {
    primary: '#2196F3',
    secondary: '#FF9800',
    success: '#4CAF50',
    error: '#F44336',
    warning: '#FF9800',
    background: '#F5F5F5',
    surface: '#FFFFFF',
    text: '#212121',
    textSecondary: '#757575',
    border: '#E0E0E0',
  },
  SPACING: {
    xs: 4,
    sm: 8,
    md: 16,
    lg: 24,
    xl: 32,
    xxl: 48,
  },
  FONT_SIZES: {
    xs: 12,
    sm: 14,
    md: 16,
    lg: 18,
    xl: 20,
    xxl: 24,
  },
} as const;
