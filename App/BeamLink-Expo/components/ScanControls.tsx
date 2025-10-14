import React, { memo, useMemo, useCallback } from 'react';
import { View, Text, StyleSheet, TouchableOpacity, ActivityIndicator } from 'react-native';
import { BLEScanState } from '../types/ble';
import { UI_CONFIG } from '../constants/ble';

interface ScanControlsProps {
  scanState: BLEScanState;
  onStartScan: () => void;
  onStopScan: () => void;
  onClearDevices: () => void;
  onToggleFreeze: () => void;
  deviceCount: number;
  isBluetoothEnabled: boolean;
  isListFrozen: boolean;
}

const ScanControls: React.FC<ScanControlsProps> = memo(({
  scanState,
  onStartScan,
  onStopScan,
  onClearDevices,
  onToggleFreeze,
  deviceCount,
  isBluetoothEnabled,
  isListFrozen,
}) => {
  const isScanning = scanState === BLEScanState.SCANNING;
  const canScan = isBluetoothEnabled && scanState !== BLEScanState.PERMISSION_DENIED;

  const buttonInfo = useMemo(() => {
    const getText = () => {
      if (!isBluetoothEnabled) return 'Enable Bluetooth';
      if (scanState === BLEScanState.PERMISSION_DENIED) return 'Permissions Required';
      return isScanning ? 'Stop Scan' : 'Start Scan';
    };

    return {
      text: getText(),
    };
  }, [isBluetoothEnabled, scanState, isScanning]);

  const statusInfo = useMemo(() => {
    const getText = () => {
      if (!isBluetoothEnabled) return 'Bluetooth is disabled';
      if (scanState === BLEScanState.PERMISSION_DENIED) return 'Bluetooth permissions required';
      if (isScanning) return 'Scanning for devices...';
      if (deviceCount > 0) return `Found ${deviceCount} device${deviceCount === 1 ? '' : 's'}`;
      return 'Ready to scan';
    };

    const getColor = () => {
      if (!isBluetoothEnabled || scanState === BLEScanState.PERMISSION_DENIED) {
        return UI_CONFIG.COLORS.error;
      }
      if (isScanning) return UI_CONFIG.COLORS.primary;
      if (deviceCount > 0) return UI_CONFIG.COLORS.success;
      return UI_CONFIG.COLORS.textSecondary;
    };

    return {
      text: getText(),
      color: getColor(),
    };
  }, [isBluetoothEnabled, scanState, isScanning, deviceCount]);

  const handlePrimaryButtonPress = useCallback(() => {
    if (isScanning) {
      onStopScan();
    } else {
      onStartScan();
    }
  }, [isScanning, onStartScan, onStopScan]);

  return (
    <View style={styles.container}>
      <View style={styles.statusContainer}>
        {isScanning && (
          <ActivityIndicator 
            size="small" 
            color={UI_CONFIG.COLORS.primary} 
            style={styles.statusIndicator}
          />
        )}
        <Text style={[styles.statusText, { color: statusInfo.color }]}>
          {statusInfo.text}
        </Text>
      </View>

      <View style={styles.controlsRow}>
        <TouchableOpacity
          style={[
            styles.primaryButton,
            !canScan && styles.disabledButton,
            isScanning && styles.stopButton,
          ]}
          onPress={handlePrimaryButtonPress}
          disabled={!canScan}
          activeOpacity={0.8}
        >
          <Text style={[
            styles.primaryButtonText,
            !canScan && styles.disabledButtonText,
            isScanning && styles.stopButtonText,
          ]}>
            {buttonInfo.text}
          </Text>
        </TouchableOpacity>

        {deviceCount > 0 && (
          <TouchableOpacity
            style={styles.secondaryButton}
            onPress={onClearDevices}
            activeOpacity={0.8}
          >
            <Text style={styles.secondaryButtonText}>Clear</Text>
          </TouchableOpacity>
        )}
      </View>

      {deviceCount > 0 && (
        <View style={styles.freezeContainer}>
          <TouchableOpacity
            style={[
              styles.freezeButton,
              isListFrozen && styles.freezeButtonActive,
            ]}
            onPress={onToggleFreeze}
            activeOpacity={0.8}
          >
            <Text style={[
              styles.freezeButtonText,
              isListFrozen && styles.freezeButtonTextActive,
            ]}>
              {isListFrozen ? '🔒 List Frozen' : '🔓 Freeze List'}
            </Text>
          </TouchableOpacity>
          <Text style={styles.freezeDescription}>
            {isListFrozen 
              ? 'Device list is frozen - no new devices will be added' 
              : 'Freeze the list to prevent reordering while scanning'
            }
          </Text>
        </View>
      )}
    </View>
  );
});

ScanControls.displayName = 'ScanControls';

const styles = StyleSheet.create({
  container: {
    backgroundColor: UI_CONFIG.COLORS.surface,
    padding: UI_CONFIG.SPACING.md,
    marginBottom: UI_CONFIG.SPACING.md,
    borderRadius: 12,
    shadowColor: '#000',
    shadowOffset: {
      width: 0,
      height: 2,
    },
    shadowOpacity: 0.1,
    shadowRadius: 3.84,
    elevation: 5,
  },
  statusContainer: {
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'center',
    marginBottom: UI_CONFIG.SPACING.md,
  },
  statusIndicator: {
    marginRight: UI_CONFIG.SPACING.sm,
  },
  statusText: {
    fontSize: UI_CONFIG.FONT_SIZES.md,
    fontWeight: '500',
    textAlign: 'center',
  },
  controlsRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
  },
  primaryButton: {
    flex: 1,
    backgroundColor: UI_CONFIG.COLORS.primary,
    paddingVertical: UI_CONFIG.SPACING.md,
    paddingHorizontal: UI_CONFIG.SPACING.lg,
    borderRadius: 8,
    alignItems: 'center',
    marginRight: UI_CONFIG.SPACING.sm,
  },
  stopButton: {
    backgroundColor: UI_CONFIG.COLORS.error,
  },
  disabledButton: {
    backgroundColor: UI_CONFIG.COLORS.border,
  },
  primaryButtonText: {
    color: 'white',
    fontSize: UI_CONFIG.FONT_SIZES.md,
    fontWeight: 'bold',
  },
  stopButtonText: {
    color: 'white',
  },
  disabledButtonText: {
    color: UI_CONFIG.COLORS.textSecondary,
  },
  secondaryButton: {
    backgroundColor: 'transparent',
    paddingVertical: UI_CONFIG.SPACING.md,
    paddingHorizontal: UI_CONFIG.SPACING.lg,
    borderRadius: 8,
    borderWidth: 1,
    borderColor: UI_CONFIG.COLORS.border,
    alignItems: 'center',
  },
  secondaryButtonText: {
    color: UI_CONFIG.COLORS.textSecondary,
    fontSize: UI_CONFIG.FONT_SIZES.md,
    fontWeight: '500',
  },
  freezeContainer: {
    marginTop: UI_CONFIG.SPACING.md,
    alignItems: 'center',
  },
  freezeButton: {
    backgroundColor: 'transparent',
    paddingVertical: UI_CONFIG.SPACING.sm,
    paddingHorizontal: UI_CONFIG.SPACING.md,
    borderRadius: 6,
    borderWidth: 1,
    borderColor: UI_CONFIG.COLORS.border,
    marginBottom: UI_CONFIG.SPACING.xs,
  },
  freezeButtonActive: {
    backgroundColor: UI_CONFIG.COLORS.primary,
    borderColor: UI_CONFIG.COLORS.primary,
  },
  freezeButtonText: {
    color: UI_CONFIG.COLORS.textSecondary,
    fontSize: UI_CONFIG.FONT_SIZES.sm,
    fontWeight: '500',
  },
  freezeButtonTextActive: {
    color: 'white',
  },
  freezeDescription: {
    color: UI_CONFIG.COLORS.textSecondary,
    fontSize: UI_CONFIG.FONT_SIZES.xs,
    textAlign: 'center',
    fontStyle: 'italic',
  },
});

export default ScanControls;
