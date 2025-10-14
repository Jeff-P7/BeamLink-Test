import React, { useState, useCallback, useRef } from 'react';
import { BleManager, Device, State } from 'react-native-ble-plx';
import { Platform, PermissionsAndroid } from 'react-native';
import { BLEDeviceInfo, BLEScanState, BLEDeviceFilter } from '../types/ble';
import { BLE_CONFIG, ESP32_CONFIG } from '../constants/ble';

export const useBLEScanning = (managerRef: React.RefObject<BleManager | null>) => {
  const [scanState, setScanState] = useState<BLEScanState>(BLEScanState.IDLE);
  const [devices, setDevices] = useState<BLEDeviceInfo[]>([]);
  const [isListFrozen, setIsListFrozen] = useState<boolean>(false);
  
  const scanTimeoutRef = useRef<NodeJS.Timeout | null>(null);

  // Convert Device to BLEDeviceInfo
  const convertToBLEDeviceInfo = useCallback((device: Device): BLEDeviceInfo => {
    const result: BLEDeviceInfo = {
      id: device.id,
      name: device.name || null,
      rssi: device.rssi,
      isConnectable: device.isConnectable ?? false,
    };

    if (device.manufacturerData) {
      result.manufacturerData = device.manufacturerData.split(' ').map(byte => `0x${byte}`).join(' ');
    }

    if (device.serviceUUIDs) {
      result.serviceUUIDs = device.serviceUUIDs;
    }

    return result;
  }, []);

  // Sort devices by RSSI (strongest signal first)
  const sortDevicesByRSSI = useCallback((deviceList: BLEDeviceInfo[]): BLEDeviceInfo[] => {
    return [...deviceList].sort((a, b) => {
      // Handle null/undefined RSSI values
      if (a.rssi === null || a.rssi === undefined) return 1;
      if (b.rssi === null || b.rssi === undefined) return -1;
      
      // Sort by RSSI (higher values = closer/stronger signal)
      return b.rssi - a.rssi;
    });
  }, []);

  // Request permissions
  const requestPermissions = useCallback(async (): Promise<boolean> => {
    if (Platform.OS !== 'android') {
      return true;
    }

    try {
      const permissions = [
        PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
        PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN,
        PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT,
      ];

      const results = await PermissionsAndroid.requestMultiple(permissions);
      
      const allGranted = Object.values(results).every(
        (result) => result === PermissionsAndroid.RESULTS.GRANTED
      );

      if (!allGranted) {
        setScanState(BLEScanState.PERMISSION_DENIED);
        return false;
      }

      return true;
    } catch (err) {
      console.error('Permission request failed:', err);
      return false;
    }
  }, []);

  // Start scanning
  const startScan = useCallback(async (filter?: BLEDeviceFilter) => {
    if (!managerRef.current) {
      return false;
    }

    if (scanState === BLEScanState.SCANNING) {
      return false;
    }

    const hasPermissions = await requestPermissions();
    if (!hasPermissions) {
      return false;
    }

    setScanState(BLEScanState.SCANNING);
    setDevices([]);
    setIsListFrozen(false); // Reset freeze state when starting new scan

    try {
      managerRef.current.startDeviceScan(null, null, (error, device) => {
        if (error) {
          console.warn('Scan error:', error);
          setScanState(BLEScanState.ERROR);
          return;
        }

        if (device) {
          const deviceInfo = convertToBLEDeviceInfo(device);
          
          setDevices(prev => {
            // Check for duplicates and update existing device
            const existingIndex = prev.findIndex(d => d.id === deviceInfo.id);
            if (existingIndex >= 0) {
              // Update existing device with latest info but maintain its position
              const updated = [...prev];
              updated[existingIndex] = deviceInfo;
              return updated;
            }
            
            // If list is frozen, don't add new devices
            if (isListFrozen) {
              return prev;
            }
            
            // Add new device if under limit
            if (prev.length >= BLE_CONFIG.MAX_DEVICES) {
              return prev;
            }
            
            // For new devices, add to list and sort by RSSI to maintain nearest-first order
            const newList = [...prev, deviceInfo];
            return sortDevicesByRSSI(newList);
          });
        }
      });

      // Auto-stop after timeout
      scanTimeoutRef.current = setTimeout(() => {
        stopScan();
      }, BLE_CONFIG.SCAN_TIMEOUT);

      return true;
    } catch (err) {
      console.error('Failed to start scan:', err);
      setScanState(BLEScanState.ERROR);
      return false;
    }
  }, [scanState, requestPermissions, convertToBLEDeviceInfo, sortDevicesByRSSI, isListFrozen]);

  // Stop scanning
  const stopScan = useCallback(() => {
    if (managerRef.current && scanState === BLEScanState.SCANNING) {
      managerRef.current.stopDeviceScan();
    }
    
    if (scanTimeoutRef.current) {
      clearTimeout(scanTimeoutRef.current);
      scanTimeoutRef.current = null;
    }
    
    setScanState(BLEScanState.IDLE);
  }, [scanState]);

  // Clear devices
  const clearDevices = useCallback(() => {
    setDevices([]);
  }, []);

  // Sort existing devices by RSSI
  const sortDevices = useCallback(() => {
    setDevices(prev => sortDevicesByRSSI(prev));
  }, [sortDevicesByRSSI]);

  // Freeze/unfreeze the device list to prevent reordering
  const freezeDeviceList = useCallback(() => {
    setIsListFrozen(true);
  }, []);

  const unfreezeDeviceList = useCallback(() => {
    setIsListFrozen(false);
  }, []);

  const toggleDeviceListFreeze = useCallback(() => {
    setIsListFrozen(prev => !prev);
  }, []);

  return {
    // State
    scanState,
    devices,
    isListFrozen,
    
    // Actions
    startScan,
    stopScan,
    clearDevices,
    sortDevices,
    freezeDeviceList,
    unfreezeDeviceList,
    toggleDeviceListFreeze,
  };
};