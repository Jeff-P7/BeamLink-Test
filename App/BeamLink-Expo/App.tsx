import React, { useState, useMemo, useCallback, memo } from 'react';
import { View, Text, FlatList, StyleSheet, StatusBar } from 'react-native';
import { useBLE } from './hooks/useBLE';
import DeviceCard from './components/DeviceCard';
import ScanControls from './components/ScanControls';
import ErrorBanner from './components/ErrorBanner';
import { BLEDeviceInfo, ESP32DeviceType, BLEScanState } from './types/ble';
import { UI_CONFIG, ESP32_CONFIG } from './constants/ble';

export default function App() {
  const {
    scanState,
    devices,
    error,
    isBluetoothEnabled,
    connectedDevice,
    isListFrozen,
    startScan,
    stopScan,
    clearDevices,
    freezeDeviceList,
    unfreezeDeviceList,
    toggleDeviceListFreeze,
    connectToDevice,
    disconnectFromDevice,
    turnOnLED,
    turnOffLED,
    toggleLED,
    getLEDStatus,
    isESP32Device,
    filterDevices,
  } = useBLE();

  const [showESP32Only, setShowESP32Only] = useState(false);
  const [dismissedError, setDismissedError] = useState<string | null>(null);

  // Filter and sort devices
  const filteredDevices = useMemo(() => {
    let filtered = devices;

    // Apply ESP32 filter if enabled
    if (showESP32Only) {
      filtered = devices.filter(device => 
        isESP32Device(device) !== ESP32DeviceType.UNKNOWN
      );
    }

    // Sort by RSSI (stronger signals first) and then by name
    return filtered.sort((a, b) => {
      if (a.rssi && b.rssi) {
        return b.rssi - a.rssi; // Higher RSSI first
      }
      if (a.rssi && !b.rssi) return -1;
      if (!a.rssi && b.rssi) return 1;
      
      const nameA = a.name || '';
      const nameB = b.name || '';
      return nameA.localeCompare(nameB);
    });
  }, [devices, showESP32Only, isESP32Device]);

  // Count ESP32 devices
  const esp32Count = useMemo(() => 
    devices.filter(device => 
      isESP32Device(device) !== ESP32DeviceType.UNKNOWN
    ).length,
    [devices, isESP32Device]
  );

  const handleDevicePress = useCallback((device: BLEDeviceInfo) => {
    console.log('Device pressed:', device);
    // Device press is now handled by DeviceCard for connection
  }, []);

  const handleConnectDevice = useCallback(async (device: BLEDeviceInfo) => {
    console.log('Connecting to device:', device);
    const success = await connectToDevice(device);
    if (success) {
      console.log('Successfully connected to device');
    } else {
      console.log('Failed to connect to device');
    }
  }, [connectToDevice]);

  const handleDisconnectDevice = useCallback(async () => {
    console.log('Disconnecting from device');
    await disconnectFromDevice();
  }, [disconnectFromDevice]);

  const handleTurnOnLED = useCallback(async () => {
    console.log('Turning on LED');
    await turnOnLED();
  }, [turnOnLED]);

  const handleTurnOffLED = useCallback(async () => {
    console.log('Turning off LED');
    await turnOffLED();
  }, [turnOffLED]);

  const handleToggleLED = useCallback(async () => {
    console.log('Toggling LED');
    await toggleLED();
  }, [toggleLED]);

  const handleGetLEDStatus = useCallback(async () => {
    console.log('Getting LED status');
    await getLEDStatus();
  }, [getLEDStatus]);

  const handleDismissError = useCallback(() => {
    setDismissedError(error);
  }, [error]);

  // Memoized render item for FlatList
  const renderDeviceItem = useCallback(({ item }: { item: BLEDeviceInfo }) => (
    <DeviceCard
      device={item}
      onPress={handleDevicePress}
      esp32Type={isESP32Device(item)}
      connectedDevice={connectedDevice}
      onConnect={handleConnectDevice}
      onDisconnect={handleDisconnectDevice}
      onTurnOnLED={handleTurnOnLED}
      onTurnOffLED={handleTurnOffLED}
      onToggleLED={handleToggleLED}
      onGetLEDStatus={handleGetLEDStatus}
    />
  ), [
    handleDevicePress,
    isESP32Device,
    connectedDevice,
    handleConnectDevice,
    handleDisconnectDevice,
    handleTurnOnLED,
    handleTurnOffLED,
    handleToggleLED,
    handleGetLEDStatus,
  ]);

  // Memoized key extractor
  const keyExtractor = useCallback((item: BLEDeviceInfo) => item.id, []);

  // Memoized empty component
  const ListEmptyComponent = useMemo(() => (
    <View style={styles.emptyContainer}>
      <Text style={styles.emptyTitle}>
        {showESP32Only ? 'No ESP32 devices found' : 'No devices found'}
      </Text>
      <Text style={styles.emptySubtitle}>
        {scanState === BLEScanState.SCANNING 
          ? 'Keep scanning...' 
          : 'Tap "Start Scan" to discover nearby devices'
        }
      </Text>
    </View>
  ), [showESP32Only, scanState]);

  // Show BLE unavailable screen
  if (scanState === BLEScanState.ERROR && !isBluetoothEnabled) {
    return (
      <View style={styles.unavailableContainer}>
        <StatusBar barStyle="dark-content" backgroundColor={UI_CONFIG.COLORS.background} />
        <View style={styles.unavailableContent}>
          <Text style={styles.title}>BLE ESP32 Scanner</Text>
          <Text style={styles.subtitle}>Bluetooth Low Energy not available</Text>
          <Text style={styles.description}>
            This app requires native BLE support.{'\n'}
            Create a development build to use BLE scanning features.
          </Text>
        </View>
      </View>
    );
  }

  return (
    <View style={styles.container}>
      <StatusBar barStyle="dark-content" backgroundColor={UI_CONFIG.COLORS.background} />
      
      {/* Header */}
      <View style={styles.header}>
        <Text style={styles.headerTitle}>BLE ESP32 LED Controller</Text>
        <Text style={styles.headerSubtitle}>
          {filteredDevices.length} device{filteredDevices.length === 1 ? '' : 's'} found
          {esp32Count > 0 && ` • ${esp32Count} ESP32`}
          {connectedDevice && ` • Connected to ${connectedDevice.name || 'ESP32'}`}
        </Text>
      </View>

      {/* Error Banner */}
      {error && error !== dismissedError && (
        <ErrorBanner 
          message={error} 
          onDismiss={handleDismissError}
          type={scanState === BLEScanState.PERMISSION_DENIED ? 'warning' : 'error'}
        />
      )}

      {/* Scan Controls */}
      <ScanControls
        scanState={scanState}
        onStartScan={startScan}
        onStopScan={stopScan}
        onClearDevices={clearDevices}
        onToggleFreeze={toggleDeviceListFreeze}
        deviceCount={devices.length}
        isBluetoothEnabled={isBluetoothEnabled}
        isListFrozen={isListFrozen}
      />

      {/* ESP32 Filter Toggle */}
      {devices.length > 0 && (
        <View style={styles.filterContainer}>
          <Text 
            style={[styles.filterToggle, showESP32Only && styles.filterToggleActive]}
            onPress={() => setShowESP32Only(!showESP32Only)}
          >
            {showESP32Only ? 'Show All Devices' : `Show ESP32 Only (${esp32Count})`}
          </Text>
        </View>
      )}

      {/* Device List */}
      <FlatList
        data={filteredDevices}
        keyExtractor={keyExtractor}
        renderItem={renderDeviceItem}
        contentContainerStyle={styles.listContainer}
        showsVerticalScrollIndicator={false}
        ListEmptyComponent={ListEmptyComponent}
        removeClippedSubviews={true}
        maxToRenderPerBatch={10}
        updateCellsBatchingPeriod={50}
        initialNumToRender={10}
        windowSize={10}
        getItemLayout={(data, index) => ({
          length: 200, // Approximate item height
          offset: 200 * index,
          index,
        })}
      />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: UI_CONFIG.COLORS.background,
  },
  unavailableContainer: {
    flex: 1,
    backgroundColor: UI_CONFIG.COLORS.background,
    justifyContent: 'center',
    alignItems: 'center',
    padding: UI_CONFIG.SPACING.lg,
  },
  unavailableContent: {
    alignItems: 'center',
  },
  title: {
    fontSize: UI_CONFIG.FONT_SIZES.xxl,
    fontWeight: 'bold',
    color: UI_CONFIG.COLORS.text,
    marginBottom: UI_CONFIG.SPACING.md,
    textAlign: 'center',
  },
  subtitle: {
    fontSize: UI_CONFIG.FONT_SIZES.lg,
    color: UI_CONFIG.COLORS.error,
    marginBottom: UI_CONFIG.SPACING.lg,
    textAlign: 'center',
  },
  description: {
    fontSize: UI_CONFIG.FONT_SIZES.md,
    color: UI_CONFIG.COLORS.textSecondary,
    textAlign: 'center',
    lineHeight: 24,
  },
  header: {
    backgroundColor: UI_CONFIG.COLORS.surface,
    paddingTop: UI_CONFIG.SPACING.xl,
    paddingBottom: UI_CONFIG.SPACING.md,
    paddingHorizontal: UI_CONFIG.SPACING.md,
    borderBottomWidth: 1,
    borderBottomColor: UI_CONFIG.COLORS.border,
  },
  headerTitle: {
    fontSize: UI_CONFIG.FONT_SIZES.xxl,
    fontWeight: 'bold',
    color: UI_CONFIG.COLORS.text,
    marginBottom: UI_CONFIG.SPACING.xs,
  },
  headerSubtitle: {
    fontSize: UI_CONFIG.FONT_SIZES.sm,
    color: UI_CONFIG.COLORS.textSecondary,
  },
  filterContainer: {
    paddingHorizontal: UI_CONFIG.SPACING.md,
    paddingVertical: UI_CONFIG.SPACING.sm,
    alignItems: 'center',
  },
  filterToggle: {
    fontSize: UI_CONFIG.FONT_SIZES.sm,
    color: UI_CONFIG.COLORS.primary,
    fontWeight: '500',
    textDecorationLine: 'underline',
  },
  filterToggleActive: {
    color: UI_CONFIG.COLORS.secondary,
    fontWeight: 'bold',
  },
  listContainer: {
    paddingBottom: UI_CONFIG.SPACING.lg,
  },
  emptyContainer: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    paddingVertical: UI_CONFIG.SPACING.xxl,
    paddingHorizontal: UI_CONFIG.SPACING.lg,
  },
  emptyTitle: {
    fontSize: UI_CONFIG.FONT_SIZES.lg,
    fontWeight: 'bold',
    color: UI_CONFIG.COLORS.text,
    marginBottom: UI_CONFIG.SPACING.sm,
    textAlign: 'center',
  },
  emptySubtitle: {
    fontSize: UI_CONFIG.FONT_SIZES.md,
    color: UI_CONFIG.COLORS.textSecondary,
    textAlign: 'center',
    lineHeight: 22,
  },
});
