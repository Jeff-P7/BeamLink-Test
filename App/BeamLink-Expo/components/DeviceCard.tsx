import React, { memo, useMemo } from 'react';
import { View, Text, StyleSheet, TouchableOpacity, ActivityIndicator } from 'react-native';
import { BLEDeviceInfo, ESP32DeviceType, BLEConnectionState, ConnectedDevice } from '../types/ble';
import { UI_CONFIG } from '../constants/ble';
import LEDControl from './LEDControl';

interface DeviceCardProps {
  device: BLEDeviceInfo;
  onPress?: (device: BLEDeviceInfo) => void;
  esp32Type?: ESP32DeviceType;
  connectedDevice?: ConnectedDevice | null;
  onConnect?: (device: BLEDeviceInfo) => void;
  onDisconnect?: () => void;
  onTurnOnLED?: () => void;
  onTurnOffLED?: () => void;
  onToggleLED?: () => void;
  onGetLEDStatus?: () => void;
}

const DeviceCard: React.FC<DeviceCardProps> = memo(({ 
  device, 
  onPress, 
  esp32Type, 
  connectedDevice,
  onConnect,
  onDisconnect,
  onTurnOnLED,
  onTurnOffLED,
  onToggleLED,
  onGetLEDStatus
}) => {
  const getRSSIColor = useMemo(() => (rssi: number | null) => {
    if (!rssi) return UI_CONFIG.COLORS.textSecondary;
    if (rssi > -50) return UI_CONFIG.COLORS.success;
    if (rssi > -70) return UI_CONFIG.COLORS.warning;
    return UI_CONFIG.COLORS.error;
  }, []);

  const getRSSIStrength = useMemo(() => (rssi: number | null) => {
    if (!rssi) return 'Unknown';
    if (rssi > -50) return 'Excellent';
    if (rssi > -60) return 'Good';
    if (rssi > -70) return 'Fair';
    return 'Weak';
  }, []);

  const getESP32BadgeColor = useMemo(() => (type: ESP32DeviceType) => {
    switch (type) {
      case ESP32DeviceType.ESP32: return UI_CONFIG.COLORS.primary;
      case ESP32DeviceType.ESP32_S2: return UI_CONFIG.COLORS.secondary;
      case ESP32DeviceType.ESP32_S3: return '#9C27B0';
      case ESP32DeviceType.ESP32_C3: return '#00BCD4';
      default: return UI_CONFIG.COLORS.textSecondary;
    }
  }, []);

  const connectionState = useMemo(() => {
    const isThisDeviceConnected = connectedDevice?.id === device.id;
    const isConnected = isThisDeviceConnected && connectedDevice?.connectionState === BLEConnectionState.CONNECTED;
    const isConnecting = isThisDeviceConnected && connectedDevice?.connectionState === BLEConnectionState.CONNECTING;
    const isOtherDeviceConnected = Boolean(connectedDevice && !isThisDeviceConnected);
    
    return { isThisDeviceConnected, isConnected, isConnecting, isOtherDeviceConnected };
  }, [connectedDevice, device.id]);

  const { isThisDeviceConnected, isConnected, isConnecting, isOtherDeviceConnected } = connectionState;

  const handleDevicePress = useMemo(() => () => {
    if (isConnected) {
      // If connected, show LED controls (handled by parent)
      onPress?.(device);
    } else if (!isOtherDeviceConnected) {
      // If not connected and no other device is connected, try to connect
      onConnect?.(device);
    }
  }, [isConnected, isOtherDeviceConnected, onPress, onConnect, device]);

  const connectionButtonProps = useMemo(() => {
    const getConnectionButtonText = () => {
      if (isConnecting) return 'Connecting...';
      if (isConnected) return 'Connected';
      if (isOtherDeviceConnected) return 'Another device connected';
      return 'Connect';
    };

    const getConnectionButtonColor = () => {
      if (isConnecting) return UI_CONFIG.COLORS.warning;
      if (isConnected) return UI_CONFIG.COLORS.success;
      if (isOtherDeviceConnected) return UI_CONFIG.COLORS.textSecondary;
      return UI_CONFIG.COLORS.primary;
    };

    return {
      text: getConnectionButtonText(),
      color: getConnectionButtonColor()
    };
  }, [isConnecting, isConnected, isOtherDeviceConnected]);

  return (
    <View style={styles.container}>
      <TouchableOpacity 
        style={styles.deviceInfo}
        onPress={handleDevicePress}
        activeOpacity={0.7}
        disabled={isOtherDeviceConnected}
      >
        <View style={styles.header}>
          <Text style={styles.deviceName}>
            {device.name || 'Unnamed Device'}
          </Text>
          {esp32Type && esp32Type !== ESP32DeviceType.UNKNOWN && (
            <View style={[styles.badge, { backgroundColor: getESP32BadgeColor(esp32Type) }]}>
              <Text style={styles.badgeText}>{esp32Type.replace('_', '-').toUpperCase()}</Text>
            </View>
          )}
        </View>
        
        <Text style={styles.deviceId}>{device.id}</Text>
        
        <View style={styles.infoRow}>
          <View style={styles.rssiContainer}>
            <Text style={styles.label}>Signal:</Text>
            <Text style={[styles.rssiValue, { color: getRSSIColor(device.rssi) }]}>
              {device.rssi ? `${device.rssi} dBm` : 'Unknown'}
            </Text>
            <Text style={[styles.rssiStrength, { color: getRSSIColor(device.rssi) }]}>
              ({getRSSIStrength(device.rssi)})
            </Text>
          </View>
          
          <View style={styles.connectableContainer}>
            <Text style={styles.label}>Connectable:</Text>
            <Text style={[
              styles.connectableValue,
              { color: device.isConnectable ? UI_CONFIG.COLORS.success : UI_CONFIG.COLORS.error }
            ]}>
              {device.isConnectable ? 'Yes' : 'No'}
            </Text>
          </View>
        </View>

        {device.manufacturerData && (
          <View style={styles.manufacturerContainer}>
            <Text style={styles.label}>Manufacturer Data:</Text>
            <Text style={styles.manufacturerData}>{device.manufacturerData}</Text>
          </View>
        )}

        {device.serviceUUIDs && device.serviceUUIDs.length > 0 && (
          <View style={styles.servicesContainer}>
            <Text style={styles.label}>Services ({device.serviceUUIDs.length}):</Text>
            {device.serviceUUIDs.slice(0, 2).map((uuid, index) => (
              <Text key={index} style={styles.serviceUUID}>{uuid}</Text>
            ))}
            {device.serviceUUIDs.length > 2 && (
              <Text style={styles.moreServices}>...and {device.serviceUUIDs.length - 2} more</Text>
            )}
          </View>
        )}
      </TouchableOpacity>

      {/* Connection Button */}
      <View style={styles.connectionContainer}>
        <TouchableOpacity
          style={[
            styles.connectionButton,
            { backgroundColor: connectionButtonProps.color },
            (isOtherDeviceConnected || isConnecting) && styles.disabledButton
          ]}
          onPress={handleDevicePress}
          disabled={isOtherDeviceConnected || isConnecting}
          activeOpacity={0.7}
        >
          {isConnecting && <ActivityIndicator size="small" color="white" style={styles.buttonSpinner} />}
          <Text style={[
            styles.connectionButtonText,
            (isOtherDeviceConnected || isConnecting) && styles.disabledButtonText
          ]}>
            {connectionButtonProps.text}
          </Text>
        </TouchableOpacity>

        {isConnected && onDisconnect && (
          <TouchableOpacity
            style={[styles.connectionButton, styles.disconnectButton]}
            onPress={onDisconnect}
            activeOpacity={0.7}
          >
            <Text style={styles.connectionButtonText}>Disconnect</Text>
          </TouchableOpacity>
        )}
      </View>

      {/* LED Controls - Only show for connected ESP32 devices */}
      {isConnected && esp32Type && esp32Type !== ESP32DeviceType.UNKNOWN && connectedDevice && (
        <LEDControl
          ledState={connectedDevice.ledState}
          connectionState={connectedDevice.connectionState}
          onTurnOn={onTurnOnLED || (() => {})}
          onTurnOff={onTurnOffLED || (() => {})}
          onToggle={onToggleLED || (() => {})}
          onGetStatus={onGetLEDStatus || (() => {})}
        />
      )}
    </View>
  );
});

const styles = StyleSheet.create({
  container: {
    backgroundColor: UI_CONFIG.COLORS.surface,
    borderRadius: 12,
    marginVertical: UI_CONFIG.SPACING.xs,
    marginHorizontal: UI_CONFIG.SPACING.sm,
    shadowColor: '#000',
    shadowOffset: {
      width: 0,
      height: 2,
    },
    shadowOpacity: 0.1,
    shadowRadius: 3.84,
    elevation: 5,
    borderLeftWidth: 4,
    borderLeftColor: UI_CONFIG.COLORS.primary,
  },
  deviceInfo: {
    padding: UI_CONFIG.SPACING.md,
  },
  header: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: UI_CONFIG.SPACING.xs,
  },
  deviceName: {
    fontSize: UI_CONFIG.FONT_SIZES.lg,
    fontWeight: 'bold',
    color: UI_CONFIG.COLORS.text,
    flex: 1,
  },
  badge: {
    paddingHorizontal: UI_CONFIG.SPACING.sm,
    paddingVertical: UI_CONFIG.SPACING.xs,
    borderRadius: 12,
    marginLeft: UI_CONFIG.SPACING.sm,
  },
  badgeText: {
    color: 'white',
    fontSize: UI_CONFIG.FONT_SIZES.xs,
    fontWeight: 'bold',
  },
  deviceId: {
    fontSize: UI_CONFIG.FONT_SIZES.sm,
    color: UI_CONFIG.COLORS.textSecondary,
    fontFamily: 'monospace',
    marginBottom: UI_CONFIG.SPACING.sm,
  },
  infoRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    marginBottom: UI_CONFIG.SPACING.sm,
  },
  rssiContainer: {
    flex: 1,
  },
  connectableContainer: {
    alignItems: 'flex-end',
  },
  label: {
    fontSize: UI_CONFIG.FONT_SIZES.xs,
    color: UI_CONFIG.COLORS.textSecondary,
    fontWeight: '500',
  },
  rssiValue: {
    fontSize: UI_CONFIG.FONT_SIZES.sm,
    fontWeight: 'bold',
  },
  rssiStrength: {
    fontSize: UI_CONFIG.FONT_SIZES.xs,
    fontStyle: 'italic',
  },
  connectableValue: {
    fontSize: UI_CONFIG.FONT_SIZES.sm,
    fontWeight: 'bold',
  },
  manufacturerContainer: {
    marginBottom: UI_CONFIG.SPACING.sm,
  },
  manufacturerData: {
    fontSize: UI_CONFIG.FONT_SIZES.xs,
    fontFamily: 'monospace',
    color: UI_CONFIG.COLORS.text,
    backgroundColor: UI_CONFIG.COLORS.background,
    padding: UI_CONFIG.SPACING.xs,
    borderRadius: 4,
    marginTop: UI_CONFIG.SPACING.xs,
  },
  servicesContainer: {
    marginTop: UI_CONFIG.SPACING.xs,
  },
  serviceUUID: {
    fontSize: UI_CONFIG.FONT_SIZES.xs,
    fontFamily: 'monospace',
    color: UI_CONFIG.COLORS.textSecondary,
    marginTop: UI_CONFIG.SPACING.xs,
  },
  moreServices: {
    fontSize: UI_CONFIG.FONT_SIZES.xs,
    color: UI_CONFIG.COLORS.textSecondary,
    fontStyle: 'italic',
    marginTop: UI_CONFIG.SPACING.xs,
  },
  connectionContainer: {
    flexDirection: 'row',
    paddingHorizontal: UI_CONFIG.SPACING.md,
    paddingBottom: UI_CONFIG.SPACING.md,
    gap: UI_CONFIG.SPACING.sm,
  },
  connectionButton: {
    flex: 1,
    paddingVertical: UI_CONFIG.SPACING.sm,
    paddingHorizontal: UI_CONFIG.SPACING.md,
    borderRadius: 8,
    alignItems: 'center',
    justifyContent: 'center',
    flexDirection: 'row',
    minHeight: 40,
  },
  disconnectButton: {
    backgroundColor: UI_CONFIG.COLORS.error,
  },
  disabledButton: {
    backgroundColor: UI_CONFIG.COLORS.border,
  },
  connectionButtonText: {
    color: 'white',
    fontSize: UI_CONFIG.FONT_SIZES.sm,
    fontWeight: 'bold',
  },
  disabledButtonText: {
    color: UI_CONFIG.COLORS.textSecondary,
  },
  buttonSpinner: {
    marginRight: UI_CONFIG.SPACING.xs,
  },
});

DeviceCard.displayName = 'DeviceCard';

export default DeviceCard;
