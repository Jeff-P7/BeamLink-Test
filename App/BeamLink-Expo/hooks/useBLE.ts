import { useState, useEffect, useCallback, useRef } from 'react';
import { BleManager, Device, State, Characteristic } from 'react-native-ble-plx';
import { Platform, PermissionsAndroid, Alert } from 'react-native';
import { BLEDeviceInfo, BLEScanState, BLEDeviceFilter, ESP32DeviceType, BLEConnectionState, LEDState, ConnectedDevice } from '../types/ble';
import { BLE_CONFIG, ESP32_CONFIG } from '../constants/ble';

export const useBLE = () => {
  const [scanState, setScanState] = useState<BLEScanState>(BLEScanState.IDLE);
  const [devices, setDevices] = useState<BLEDeviceInfo[]>([]);
  const [error, setError] = useState<string | null>(null);
  const [isBluetoothEnabled, setIsBluetoothEnabled] = useState<boolean>(false);
  const [connectedDevice, setConnectedDevice] = useState<ConnectedDevice | null>(null);
  const [isListFrozen, setIsListFrozen] = useState<boolean>(false);
  
  const managerRef = useRef<BleManager | null>(null);
  const scanTimeoutRef = useRef<NodeJS.Timeout | null>(null);
  const deviceRef = useRef<Device | null>(null);
  const characteristicRef = useRef<Characteristic | null>(null);

  // Initialize BLE Manager
  useEffect(() => {
    try {
      managerRef.current = new BleManager();
      
      // Listen for state changes
      const subscription = managerRef.current.onStateChange((state: State) => {
        setIsBluetoothEnabled(state === 'PoweredOn');
        if (state !== 'PoweredOn') {
          setError(`Bluetooth is ${state}. Please enable Bluetooth.`);
        } else {
          setError(null);
        }
      }, true);

      return () => {
        subscription.remove();
        if (managerRef.current) {
          managerRef.current.destroy();
        }
        if (scanTimeoutRef.current) {
          clearTimeout(scanTimeoutRef.current);
        }
      };
    } catch (err) {
      setError('BLE Manager not available. This app requires a development build.');
      console.warn('BLE Manager initialization failed:', err);
      return () => {}; // Return empty cleanup function
    }
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
        setError('Bluetooth permissions are required for device scanning.');
        setScanState(BLEScanState.PERMISSION_DENIED);
        return false;
      }

      setError(null);
      return true;
    } catch (err) {
      setError('Failed to request permissions');
      console.error('Permission request failed:', err);
      return false;
    }
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

  // Start scanning
  const startScan = useCallback(async (filter?: BLEDeviceFilter) => {
    if (!managerRef.current) {
      setError('BLE Manager not available');
      return;
    }

    if (scanState === BLEScanState.SCANNING) {
      return;
    }

    const hasPermissions = await requestPermissions();
    if (!hasPermissions) {
      return;
    }

    if (!isBluetoothEnabled) {
      setError('Please enable Bluetooth to scan for devices');
      return;
    }

    setScanState(BLEScanState.SCANNING);
    setError(null);
    setDevices([]);
    setIsListFrozen(false); // Reset freeze state when starting new scan

    try {
      managerRef.current.startDeviceScan(null, null, (error, device) => {
        if (error) {
          console.warn('Scan error:', error);
          setError(`Scan error: ${error.message}`);
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

    } catch (err) {
      setError(`Failed to start scan: ${err}`);
      setScanState(BLEScanState.ERROR);
    }
  }, [scanState, isBluetoothEnabled, requestPermissions, convertToBLEDeviceInfo, sortDevicesByRSSI, isListFrozen]);

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
    setError(null);
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

  // Connect to device with retry logic
  const connectToDevice = useCallback(async (deviceInfo: BLEDeviceInfo, retryCount = 0) => {
    if (!managerRef.current) {
      setError('BLE Manager not available');
      return false;
    }

    if (connectedDevice) {
      setError('Already connected to a device. Disconnect first.');
      return false;
    }

    setError(null);
    setConnectedDevice({
      ...deviceInfo,
      connectionState: BLEConnectionState.CONNECTING,
      ledState: LEDState.UNKNOWN,
    });

    try {
      const device = await managerRef.current.connectToDevice(deviceInfo.id);
      deviceRef.current = device;

      // Discover services and characteristics
      const deviceWithServices = await device.discoverAllServicesAndCharacteristics();
      
      // Find the LED control characteristics (RX for write, TX for notify)
      const services = await deviceWithServices.services();
      let ledRxCharacteristic: Characteristic | null = null;
      let ledTxCharacteristic: Characteristic | null = null;

      // Debug: Log all services and characteristics
      console.log('Discovered services:');
      for (const service of services) {
        console.log(`  Service: ${service.uuid}`);
        const characteristics = await service.characteristics();
        for (const char of characteristics) {
          console.log(`    Characteristic: ${char.uuid}`);
        }
      }

      // Check if this is a BeamLink device by looking for our service
      let isBeamLinkDevice = false;
      for (const service of services) {
        if (service.uuid.toLowerCase() === ESP32_CONFIG.LED_SERVICE_UUID.toLowerCase()) {
          isBeamLinkDevice = true;
          break;
        }
      }

      if (!isBeamLinkDevice) {
        throw new Error(`Device does not support BeamLink protocol. Found ${services.length} services but none match BeamLink service UUID ${ESP32_CONFIG.LED_SERVICE_UUID}. This may be a generic ESP32 device.`);
      }

      // Find the LED control characteristics
      let ledRxCharacteristic: Characteristic | null = null;
      let ledTxCharacteristic: Characteristic | null = null;
      
      for (const service of services) {
        if (service.uuid.toLowerCase() === ESP32_CONFIG.LED_SERVICE_UUID.toLowerCase()) {
          const characteristics = await service.characteristics();
          ledRxCharacteristic = characteristics.find(
            char => char.uuid.toLowerCase() === ESP32_CONFIG.LED_RX_CHARACTERISTIC_UUID.toLowerCase()
          ) || null;
          ledTxCharacteristic = characteristics.find(
            char => char.uuid.toLowerCase() === ESP32_CONFIG.LED_TX_CHARACTERISTIC_UUID.toLowerCase()
          ) || null;
          break;
        }
      }

      if (!ledRxCharacteristic || !ledTxCharacteristic) {
        const missingChars = [];
        if (!ledRxCharacteristic) missingChars.push('RX (write)');
        if (!ledTxCharacteristic) missingChars.push('TX (notify)');
        
        throw new Error(`LED control characteristics not found: ${missingChars.join(', ')}. Looking for service ${ESP32_CONFIG.LED_SERVICE_UUID}. This device may not be running BeamLink firmware.`);
      }

      characteristicRef.current = ledRxCharacteristic;

      // Set up notification listener on TX characteristic
      ledTxCharacteristic.monitor((error, characteristic) => {
        if (error) {
          console.error('Characteristic monitoring error:', error);
          return;
        }

        if (characteristic?.value) {
          // Convert base64 to string without using Buffer
          const response = atob(characteristic.value);
          console.log('Received response:', response);
          
          setConnectedDevice(prev => {
            if (!prev) return null;
            
            let newLedState = prev.ledState;
            if (response === ESP32_CONFIG.LED_RESPONSES.LED_ON) {
              newLedState = LEDState.ON;
            } else if (response === ESP32_CONFIG.LED_RESPONSES.LED_OFF) {
              newLedState = LEDState.OFF;
            }

            return {
              ...prev,
              ledState: newLedState,
              lastResponse: response,
            };
          });
        }
      });

      // Update connection state
      setConnectedDevice(prev => prev ? {
        ...prev,
        connectionState: BLEConnectionState.CONNECTED,
      } : null);

      return true;
    } catch (err) {
      console.error('Connection error:', err);
      
      // Provide more specific error messages
      let errorMessage = 'Failed to connect';
      if (err instanceof Error) {
        if (err.message.includes('characteristics not found')) {
          errorMessage = 'Device not compatible - missing BeamLink service';
        } else if (err.message.includes('timeout')) {
          errorMessage = 'Connection timeout - device may be busy';
        } else if (err.message.includes('permission')) {
          errorMessage = 'Bluetooth permission denied';
        } else {
          errorMessage = `Connection failed: ${err.message}`;
        }
      }
      
      setError(errorMessage);
      setConnectedDevice(null);
      deviceRef.current = null;
      characteristicRef.current = null;
      
      // Retry logic for certain types of errors
      if (retryCount < BLE_CONFIG.RETRY_ATTEMPTS && 
          (err instanceof Error && 
           (err.message.includes('timeout') || err.message.includes('connection')))) {
        console.log(`Retrying connection (attempt ${retryCount + 1}/${BLE_CONFIG.RETRY_ATTEMPTS})`);
        await new Promise(resolve => setTimeout(resolve, 1000)); // Wait 1 second before retry
        return connectToDevice(deviceInfo, retryCount + 1);
      }
      
      return false;
    }
  }, [connectedDevice]);

  // Disconnect from device
  const disconnectFromDevice = useCallback(async () => {
    if (deviceRef.current) {
      try {
        await deviceRef.current.cancelConnection();
      } catch (err) {
        console.error('Disconnect error:', err);
      }
    }

    setConnectedDevice(null);
    deviceRef.current = null;
    characteristicRef.current = null;
    setError(null);
  }, []);

  // Send LED command
  const sendLEDCommand = useCallback(async (command: string) => {
    if (!characteristicRef.current || !connectedDevice) {
      setError('Not connected to device');
      return false;
    }

    if (connectedDevice.connectionState !== BLEConnectionState.CONNECTED) {
      setError('Device not connected');
      return false;
    }

    try {
      // Convert string to base64 without using Buffer
      const commandBase64 = btoa(command);
      await characteristicRef.current.writeWithResponse(commandBase64);

      setConnectedDevice(prev => prev ? {
        ...prev,
        lastCommand: command,
      } : null);

      return true;
    } catch (err) {
      console.error('Command send error:', err);
      setError(`Failed to send command: ${err}`);
      return false;
    }
  }, [connectedDevice]);

  // LED control functions
  const turnOnLED = useCallback(() => sendLEDCommand(ESP32_CONFIG.LED_COMMANDS.ON), [sendLEDCommand]);
  const turnOffLED = useCallback(() => sendLEDCommand(ESP32_CONFIG.LED_COMMANDS.OFF), [sendLEDCommand]);
  const toggleLED = useCallback(() => sendLEDCommand(ESP32_CONFIG.LED_COMMANDS.TOGGLE), [sendLEDCommand]);
  const getLEDStatus = useCallback(() => sendLEDCommand(ESP32_CONFIG.LED_COMMANDS.STATUS), [sendLEDCommand]);

  return {
    // State
    scanState,
    devices,
    error,
    isBluetoothEnabled,
    connectedDevice,
    isListFrozen,
    
    // Actions
    startScan,
    stopScan,
    clearDevices,
    sortDevices,
    freezeDeviceList,
    unfreezeDeviceList,
    toggleDeviceListFreeze,
    connectToDevice,
    disconnectFromDevice,
    
    // LED Control
    turnOnLED,
    turnOffLED,
    toggleLED,
    getLEDStatus,
    
    // Utilities
    filterDevices,
    isESP32Device,
    requestPermissions,
  };
};
