import { renderHook, act } from '@testing-library/react-hooks';
import { useBLE } from '../hooks/useBLE';

// Mock react-native-ble-plx
jest.mock('react-native-ble-plx', () => ({
  BleManager: jest.fn().mockImplementation(() => ({
    onStateChange: jest.fn(() => ({ remove: jest.fn() })),
    destroy: jest.fn(),
    startDeviceScan: jest.fn(),
    stopDeviceScan: jest.fn(),
    connectToDevice: jest.fn(),
  })),
  State: {
    PoweredOn: 'PoweredOn',
    PoweredOff: 'PoweredOff',
  },
}));

// Mock react-native
jest.mock('react-native', () => ({
  Platform: {
    OS: 'ios',
  },
  PermissionsAndroid: {
    requestMultiple: jest.fn(),
    RESULTS: {
      GRANTED: 'granted',
    },
  },
  Alert: {
    alert: jest.fn(),
  },
}));

describe('useBLE', () => {
  beforeEach(() => {
    jest.clearAllMocks();
  });

  it('should initialize with default state', () => {
    const { result } = renderHook(() => useBLE());

    expect(result.current.scanState).toBe('idle');
    expect(result.current.devices).toEqual([]);
    expect(result.current.error).toBeNull();
    expect(result.current.isBluetoothEnabled).toBe(false);
    expect(result.current.connectedDevice).toBeNull();
    expect(result.current.isListFrozen).toBe(false);
  });

  it('should provide all required functions', () => {
    const { result } = renderHook(() => useBLE());

    expect(typeof result.current.startScan).toBe('function');
    expect(typeof result.current.stopScan).toBe('function');
    expect(typeof result.current.clearDevices).toBe('function');
    expect(typeof result.current.connectToDevice).toBe('function');
    expect(typeof result.current.disconnectFromDevice).toBe('function');
    expect(typeof result.current.turnOnLED).toBe('function');
    expect(typeof result.current.turnOffLED).toBe('function');
    expect(typeof result.current.toggleLED).toBe('function');
    expect(typeof result.current.getLEDStatus).toBe('function');
    expect(typeof result.current.filterDevices).toBe('function');
    expect(typeof result.current.isESP32Device).toBe('function');
  });

  it('should handle device filtering correctly', () => {
    const { result } = renderHook(() => useBLE());

    const devices = [
      { id: '1', name: 'ESP32-Device', rssi: -50, isConnectable: true },
      { id: '2', name: 'Other-Device', rssi: -70, isConnectable: true },
    ];

    const filtered = result.current.filterDevices(devices, {
      namePattern: 'ESP32',
    });

    expect(filtered).toHaveLength(1);
    expect(filtered[0].name).toBe('ESP32-Device');
  });

  it('should identify ESP32 devices correctly', () => {
    const { result } = renderHook(() => useBLE());

    const esp32Device = {
      id: '1',
      name: 'ESP32-LED-Controller',
      rssi: -50,
      isConnectable: true,
    };

    const otherDevice = {
      id: '2',
      name: 'Other-Device',
      rssi: -70,
      isConnectable: true,
    };

    expect(result.current.isESP32Device(esp32Device)).toBe('esp32');
    expect(result.current.isESP32Device(otherDevice)).toBe('unknown');
  });
});