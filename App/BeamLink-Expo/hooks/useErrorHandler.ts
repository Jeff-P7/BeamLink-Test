import { useState, useCallback, useRef } from 'react';
import { Alert } from 'react-native';

export interface ErrorInfo {
  id: string;
  message: string;
  type: 'error' | 'warning' | 'info';
  timestamp: Date;
  context?: string;
  retryable?: boolean;
}

export const useErrorHandler = () => {
  const [errors, setErrors] = useState<ErrorInfo[]>([]);
  const [currentError, setCurrentError] = useState<ErrorInfo | null>(null);
  const errorIdCounter = useRef(0);

  const addError = useCallback((
    message: string,
    type: 'error' | 'warning' | 'info' = 'error',
    context?: string,
    retryable: boolean = false
  ) => {
    const errorId = `error_${++errorIdCounter.current}`;
    const errorInfo: ErrorInfo = {
      id: errorId,
      message,
      type,
      timestamp: new Date(),
      context,
      retryable,
    };

    setErrors(prev => [...prev, errorInfo]);
    setCurrentError(errorInfo);

    // Auto-dismiss info messages after 3 seconds
    if (type === 'info') {
      setTimeout(() => {
        dismissError(errorId);
      }, 3000);
    }

    return errorId;
  }, []);

  const dismissError = useCallback((errorId: string) => {
    setErrors(prev => prev.filter(error => error.id !== errorId));
    setCurrentError(prev => prev?.id === errorId ? null : prev);
  }, []);

  const clearAllErrors = useCallback(() => {
    setErrors([]);
    setCurrentError(null);
  }, []);

  const showErrorAlert = useCallback((
    title: string,
    message: string,
    onRetry?: () => void,
    onDismiss?: () => void
  ) => {
    const buttons = [];
    
    if (onRetry) {
      buttons.push({
        text: 'Retry',
        onPress: onRetry,
        style: 'default' as const,
      });
    }
    
    buttons.push({
      text: 'OK',
      onPress: onDismiss,
      style: 'cancel' as const,
    });

    Alert.alert(title, message, buttons);
  }, []);

  const handleBLEError = useCallback((error: any, context?: string) => {
    let message = 'An unknown error occurred';
    let retryable = false;

    if (typeof error === 'string') {
      message = error;
    } else if (error?.message) {
      message = error.message;
    } else if (error?.errorMessage) {
      message = error.errorMessage;
    }

    // Determine if error is retryable based on common BLE error patterns
    if (message.includes('timeout') || 
        message.includes('connection') || 
        message.includes('scan')) {
      retryable = true;
    }

    const errorId = addError(message, 'error', context, retryable);
    
    if (retryable) {
      showErrorAlert(
        'Connection Error',
        message,
        () => {
          // Retry logic would be implemented by the caller
          dismissError(errorId);
        },
        () => dismissError(errorId)
      );
    }

    return errorId;
  }, [addError, showErrorAlert, dismissError]);

  const handlePermissionError = useCallback((permission: string) => {
    const message = `${permission} permission is required for this feature`;
    addError(message, 'warning', 'permissions');
    
    showErrorAlert(
      'Permission Required',
      message,
      undefined,
      () => clearAllErrors()
    );
  }, [addError, showErrorAlert, clearAllErrors]);

  const handleBluetoothError = useCallback((state: string) => {
    const message = `Bluetooth is ${state}. Please enable Bluetooth to continue.`;
    addError(message, 'error', 'bluetooth');
  }, [addError]);

  return {
    // State
    errors,
    currentError,
    
    // Actions
    addError,
    dismissError,
    clearAllErrors,
    
    // Specialized handlers
    handleBLEError,
    handlePermissionError,
    handleBluetoothError,
    showErrorAlert,
  };
};