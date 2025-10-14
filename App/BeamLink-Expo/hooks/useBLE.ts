import { useState, useEffect, useCallback, useRef } from 'react';
import { BleManager, State } from 'react-native-ble-plx';
import { BLEDeviceInfo, BLEScanState, BLEDeviceFilter, ESP32DeviceType } from '../types/ble';
import { ESP32_CONFIG } from '../constants/ble';
import { useBLEScanning } from './useBLEScanning';
import { useBLEConnection } from './useBLEConnection';
import { useErrorHandler } from './useErrorHandler';

export const useBLE = () => {
  const [isBluetoothEnabled, setIsBluetoothEnabled] = useState<boolean>(false);
  
  const managerRef = useRef<BleManager | null>(null);
  
  // Use specialized hooks
  const scanning = useBLEScanning(managerRef);
  const connection = useBLEConnection();
  const errorHandler = useErrorHandler();

  // Initialize BLE Manager
  useEffect(() => {
    try {
      managerRef.current = new BleManager();
      
      // Listen for state changes
      const subscription = managerRef.current.onStateChange((state: State) => {
        setIsBluetoothEnabled(state === 'PoweredOn');
        if (state !== 'PoweredOn') {
          errorHandler.handleBluetoothError(state);
        } else {
          errorHandler.clearAllErrors();
        }
      }, true);

      return () => {
        subscription.remove();
        if (managerRef.current) {
          managerRef.current.destroy();
        }
      };
    } catch (err) {
      errorHandler.handleBLEError(err, 'BLE Manager initialization');
      console.warn('BLE Manager initialization failed:', err);
      return () => {}; // Return empty cleanup function
    }
  }, []);

  // Check if device is ESP32
  const isESP32Device = useCallback((device: BLEDeviceInfo): ESP32DeviceType => {
    if (!device.name && !device.manufacturerData) {
      return ESP32DeviceType.UNKNOWN;
    }

    const name = device.name?.toUpperCase() || '';
    const isNameMatch = ESP32_CONFIG.NAME_PATTERNS.some(pattern => 
      name.includes(pattern.toUpperCase())
    );

    if (isNameMatch) {
      if (name.includes('S2')) return ESP32DeviceType.ESP32_S2;
      if (name.includes('S3')) return ESP32DeviceType.ESP32_S3;
      if (name.includes('C3')) return ESP32DeviceType.ESP32_C3;
      return ESP32DeviceType.ESP32;
    }

    // Check manufacturer data for Espressif
    if (device.manufacturerData) {
      const hasEspressifId = ESP32_CONFIG.MANUFACTURER_IDS.some(id => 
        device.manufacturerData?.includes(id)
      );
      if (hasEspressifId) return ESP32DeviceType.ESP32;
    }

    return ESP32DeviceType.UNKNOWN;
  }, []);

  // Filter devices based on criteria
  const filterDevices = useCallback((deviceList: BLEDeviceInfo[], filter?: BLEDeviceFilter): BLEDeviceInfo[] => {
    if (!filter) return deviceList;

    return deviceList.filter(device => {
      // Name pattern filter
      if (filter.namePattern && device.name) {
        const regex = new RegExp(filter.namePattern, 'i');
        if (!regex.test(device.name)) return false;
      }

      // RSSI filter
      if (filter.minRSSI && device.rssi && device.rssi < filter.minRSSI) {
        return false;
      }

      // Service UUID filter
      if (filter.serviceUUIDs && device.serviceUUIDs) {
        const hasMatchingService = filter.serviceUUIDs.some(uuid => 
          device.serviceUUIDs?.includes(uuid)
        );
        if (!hasMatchingService) return false;
      }

      return true;
    });
  }, []);

  // Enhanced start scan with error handling
  const startScan = useCallback(async (filter?: BLEDeviceFilter) => {
    if (!isBluetoothEnabled) {
      errorHandler.handleBluetoothError('disabled');
      return;
    }

    const success = await scanning.startScan(filter);
    if (!success) {
      errorHandler.handleBLEError('Failed to start scanning', 'scanning');
    }
  }, [isBluetoothEnabled, scanning, errorHandler]);

  // Enhanced connect to device with error handling
  const connectToDevice = useCallback(async (deviceInfo: BLEDeviceInfo) => {
    if (!managerRef.current) {
      errorHandler.handleBLEError('BLE Manager not available', 'connection');
      return false;
    }

    if (connection.connectedDevice) {
      errorHandler.addError('Already connected to a device. Disconnect first.', 'warning', 'connection');
      return false;
    }

    errorHandler.clearAllErrors();
    const success = await connection.connectToDevice(deviceInfo, managerRef.current);
    if (!success) {
      errorHandler.handleBLEError('Failed to connect to device', 'connection');
    }
    return success;
  }, [connection, managerRef, errorHandler]);

  // Enhanced disconnect with error handling
  const disconnectFromDevice = useCallback(async () => {
    try {
      await connection.disconnectFromDevice();
      errorHandler.clearAllErrors();
    } catch (err) {
      errorHandler.handleBLEError(err, 'disconnection');
    }
  }, [connection, errorHandler]);

  return {
    // State
    scanState: scanning.scanState,
    devices: scanning.devices,
    error: errorHandler.currentError?.message || null,
    isBluetoothEnabled,
    connectedDevice: connection.connectedDevice,
    isListFrozen: scanning.isListFrozen,
    
    // Actions
    startScan,
    stopScan: scanning.stopScan,
    clearDevices: scanning.clearDevices,
    sortDevices: scanning.sortDevices,
    freezeDeviceList: scanning.freezeDeviceList,
    unfreezeDeviceList: scanning.unfreezeDeviceList,
    toggleDeviceListFreeze: scanning.toggleDeviceListFreeze,
    connectToDevice,
    disconnectFromDevice,
    
    // LED Control
    turnOnLED: connection.turnOnLED,
    turnOffLED: connection.turnOffLED,
    toggleLED: connection.toggleLED,
    getLEDStatus: connection.getLEDStatus,
    
    // Utilities
    filterDevices,
    isESP32Device,
    
    // Error handling
    dismissError: errorHandler.dismissError,
    clearAllErrors: errorHandler.clearAllErrors,
  };
};
