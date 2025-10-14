import React, { memo, useMemo, useCallback } from 'react';
import { View, Text, StyleSheet, TouchableOpacity, ActivityIndicator } from 'react-native';
import { LEDState, BLEConnectionState } from '../types/ble';
import { UI_CONFIG } from '../constants/ble';

interface LEDControlProps {
  ledState: LEDState;
  connectionState: BLEConnectionState;
  onTurnOn: () => void;
  onTurnOff: () => void;
  onToggle: () => void;
  onGetStatus: () => void;
  disabled?: boolean;
}

const LEDControl: React.FC<LEDControlProps> = memo(({
  ledState,
  connectionState,
  onTurnOn,
  onTurnOff,
  onToggle,
  onGetStatus,
  disabled = false,
}) => {
  const isConnected = connectionState === BLEConnectionState.CONNECTED;
  const isConnecting = connectionState === BLEConnectionState.CONNECTING;
  const isDisabled = disabled || !isConnected || isConnecting;

  const ledStatusInfo = useMemo(() => {
    const getColor = () => {
      switch (ledState) {
        case LEDState.ON: return UI_CONFIG.COLORS.success;
        case LEDState.OFF: return UI_CONFIG.COLORS.error;
        default: return UI_CONFIG.COLORS.textSecondary;
      }
    };

    const getText = () => {
      switch (ledState) {
        case LEDState.ON: return 'ON';
        case LEDState.OFF: return 'OFF';
        default: return 'UNKNOWN';
      }
    };

    return {
      color: getColor(),
      text: getText(),
    };
  }, [ledState]);

  const connectionStatusInfo = useMemo(() => {
    const getText = () => {
      switch (connectionState) {
        case BLEConnectionState.CONNECTED: return 'Connected';
        case BLEConnectionState.CONNECTING: return 'Connecting...';
        case BLEConnectionState.DISCONNECTING: return 'Disconnecting...';
        case BLEConnectionState.ERROR: return 'Connection Error';
        default: return 'Disconnected';
      }
    };

    const getColor = () => {
      switch (connectionState) {
        case BLEConnectionState.CONNECTED: return UI_CONFIG.COLORS.success;
        case BLEConnectionState.CONNECTING: return UI_CONFIG.COLORS.warning;
        case BLEConnectionState.DISCONNECTING: return UI_CONFIG.COLORS.warning;
        case BLEConnectionState.ERROR: return UI_CONFIG.COLORS.error;
        default: return UI_CONFIG.COLORS.textSecondary;
      }
    };

    return {
      text: getText(),
      color: getColor(),
    };
  }, [connectionState]);

  const handleTurnOn = useCallback(() => {
    if (!isDisabled) onTurnOn();
  }, [isDisabled, onTurnOn]);

  const handleTurnOff = useCallback(() => {
    if (!isDisabled) onTurnOff();
  }, [isDisabled, onTurnOff]);

  const handleToggle = useCallback(() => {
    if (!isDisabled) onToggle();
  }, [isDisabled, onToggle]);

  const handleGetStatus = useCallback(() => {
    if (!isDisabled) onGetStatus();
  }, [isDisabled, onGetStatus]);

  return (
    <View style={styles.container}>
      {/* Status Header */}
      <View style={styles.statusHeader}>
        <View style={styles.statusRow}>
          <Text style={styles.statusLabel}>Connection:</Text>
          <View style={styles.statusValueContainer}>
            {isConnecting && <ActivityIndicator size="small" color={UI_CONFIG.COLORS.warning} />}
            <Text style={[styles.statusValue, { color: connectionStatusInfo.color }]}>
              {connectionStatusInfo.text}
            </Text>
          </View>
        </View>
        
        <View style={styles.statusRow}>
          <Text style={styles.statusLabel}>LED Status:</Text>
          <View style={styles.ledStatusContainer}>
            <View style={[styles.ledIndicator, { backgroundColor: ledStatusInfo.color }]} />
            <Text style={[styles.statusValue, { color: ledStatusInfo.color }]}>
              {ledStatusInfo.text}
            </Text>
          </View>
        </View>
      </View>

      {/* Control Buttons */}
      <View style={styles.controlsContainer}>
        <View style={styles.buttonRow}>
          <TouchableOpacity
            style={[
              styles.controlButton,
              styles.onButton,
              isDisabled && styles.disabledButton
            ]}
            onPress={handleTurnOn}
            disabled={isDisabled}
            activeOpacity={0.7}
          >
            <Text style={[styles.buttonText, isDisabled && styles.disabledButtonText]}>
              TURN ON
            </Text>
          </TouchableOpacity>

          <TouchableOpacity
            style={[
              styles.controlButton,
              styles.offButton,
              isDisabled && styles.disabledButton
            ]}
            onPress={handleTurnOff}
            disabled={isDisabled}
            activeOpacity={0.7}
          >
            <Text style={[styles.buttonText, isDisabled && styles.disabledButtonText]}>
              TURN OFF
            </Text>
          </TouchableOpacity>
        </View>

        <View style={styles.buttonRow}>
          <TouchableOpacity
            style={[
              styles.controlButton,
              styles.toggleButton,
              isDisabled && styles.disabledButton
            ]}
            onPress={handleToggle}
            disabled={isDisabled}
            activeOpacity={0.7}
          >
            <Text style={[styles.buttonText, isDisabled && styles.disabledButtonText]}>
              TOGGLE
            </Text>
          </TouchableOpacity>

          <TouchableOpacity
            style={[
              styles.controlButton,
              styles.statusButton,
              isDisabled && styles.disabledButton
            ]}
            onPress={handleGetStatus}
            disabled={isDisabled}
            activeOpacity={0.7}
          >
            <Text style={[styles.buttonText, isDisabled && styles.disabledButtonText]}>
              GET STATUS
            </Text>
          </TouchableOpacity>
        </View>
      </View>
    </View>
  );
});

LEDControl.displayName = 'LEDControl';

const styles = StyleSheet.create({
  container: {
    backgroundColor: UI_CONFIG.COLORS.surface,
    borderRadius: 12,
    padding: UI_CONFIG.SPACING.md,
    marginVertical: UI_CONFIG.SPACING.sm,
    marginHorizontal: UI_CONFIG.SPACING.sm,
    shadowColor: '#000',
    shadowOffset: {
      width: 0,
      height: 2,
    },
    shadowOpacity: 0.1,
    shadowRadius: 3.84,
    elevation: 5,
  },
  statusHeader: {
    marginBottom: UI_CONFIG.SPACING.md,
  },
  statusRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: UI_CONFIG.SPACING.sm,
  },
  statusLabel: {
    fontSize: UI_CONFIG.FONT_SIZES.sm,
    color: UI_CONFIG.COLORS.textSecondary,
    fontWeight: '500',
  },
  statusValueContainer: {
    flexDirection: 'row',
    alignItems: 'center',
  },
  statusValue: {
    fontSize: UI_CONFIG.FONT_SIZES.sm,
    fontWeight: 'bold',
    marginLeft: UI_CONFIG.SPACING.xs,
  },
  ledStatusContainer: {
    flexDirection: 'row',
    alignItems: 'center',
  },
  ledIndicator: {
    width: 12,
    height: 12,
    borderRadius: 6,
    marginRight: UI_CONFIG.SPACING.xs,
  },
  controlsContainer: {
    gap: UI_CONFIG.SPACING.sm,
  },
  buttonRow: {
    flexDirection: 'row',
    gap: UI_CONFIG.SPACING.sm,
  },
  controlButton: {
    flex: 1,
    paddingVertical: UI_CONFIG.SPACING.md,
    paddingHorizontal: UI_CONFIG.SPACING.sm,
    borderRadius: 8,
    alignItems: 'center',
    justifyContent: 'center',
    minHeight: 48,
  },
  onButton: {
    backgroundColor: UI_CONFIG.COLORS.success,
  },
  offButton: {
    backgroundColor: UI_CONFIG.COLORS.error,
  },
  toggleButton: {
    backgroundColor: UI_CONFIG.COLORS.primary,
  },
  statusButton: {
    backgroundColor: UI_CONFIG.COLORS.secondary,
  },
  disabledButton: {
    backgroundColor: UI_CONFIG.COLORS.border,
  },
  buttonText: {
    color: 'white',
    fontSize: UI_CONFIG.FONT_SIZES.sm,
    fontWeight: 'bold',
  },
  disabledButtonText: {
    color: UI_CONFIG.COLORS.textSecondary,
  },
});

export default LEDControl;
