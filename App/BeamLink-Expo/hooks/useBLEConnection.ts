import { useState, useCallback, useRef } from 'react';
import { Device, Characteristic } from 'react-native-ble-plx';
import { BLEDeviceInfo, BLEConnectionState, LEDState, ConnectedDevice } from '../types/ble';
import { ESP32_CONFIG } from '../constants/ble';

export const useBLEConnection = () => {
  const [connectedDevice, setConnectedDevice] = useState<ConnectedDevice | null>(null);
  
  const deviceRef = useRef<Device | null>(null);
  const characteristicRef = useRef<Characteristic | null>(null);

  // Connect to device
  const connectToDevice = useCallback(async (
    deviceInfo: BLEDeviceInfo, 
    manager: any
  ): Promise<boolean> => {
    if (!manager) {
      return false;
    }

    if (connectedDevice) {
      return false;
    }

    setConnectedDevice({
      ...deviceInfo,
      connectionState: BLEConnectionState.CONNECTING,
      ledState: LEDState.UNKNOWN,
    });

    try {
      const device = await manager.connectToDevice(deviceInfo.id);
      deviceRef.current = device;

      // Discover services and characteristics
      const deviceWithServices = await device.discoverAllServicesAndCharacteristics();
      
      // Find the LED control characteristics (RX for write, TX for notify)
      const services = await deviceWithServices.services();
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
        throw new Error('LED control characteristics not found');
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
      setConnectedDevice(null);
      deviceRef.current = null;
      characteristicRef.current = null;
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
  }, []);

  // Send LED command
  const sendLEDCommand = useCallback(async (command: string): Promise<boolean> => {
    if (!characteristicRef.current || !connectedDevice) {
      return false;
    }

    if (connectedDevice.connectionState !== BLEConnectionState.CONNECTED) {
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
    connectedDevice,
    
    // Actions
    connectToDevice,
    disconnectFromDevice,
    
    // LED Control
    turnOnLED,
    turnOffLED,
    toggleLED,
    getLEDStatus,
  };
};