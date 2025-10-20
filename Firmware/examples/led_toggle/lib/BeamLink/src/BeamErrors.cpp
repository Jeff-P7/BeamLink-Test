#include "BeamErrors.h"

namespace BeamErrors {

const char* toString(ErrorCode code) {
  switch (code) {
    // Success
    case ErrorCode::OK:
      return "Success";
    
    // Initialization Errors
    case ErrorCode::ALREADY_INITIALIZED:
      return "Already initialized";
    case ErrorCode::INVALID_DEVICE_NAME:
      return "Invalid device name";
    case ErrorCode::BLE_INIT_FAILED:
      return "BLE initialization failed";
    case ErrorCode::SERVER_CREATE_FAILED:
      return "Failed to create BLE server";
    case ErrorCode::SERVICE_CREATE_FAILED:
      return "Failed to create BLE service";
    case ErrorCode::CHARACTERISTIC_CREATE_FAILED:
      return "Failed to create characteristic";
    case ErrorCode::ADVERTISING_FAILED:
      return "Failed to start advertising";
    
    // Connection Errors
    case ErrorCode::NOT_CONNECTED:
      return "No client connected";
    case ErrorCode::CONNECTION_LOST:
      return "Connection lost";
    case ErrorCode::DISCONNECTED:
      return "Client disconnected";
    
    // Message Errors
    case ErrorCode::MESSAGE_TOO_LARGE:
      return "Message exceeds MTU size";
    case ErrorCode::MESSAGE_EMPTY:
      return "Message is empty";
    case ErrorCode::NOTIFY_FAILED:
      return "Failed to send notification";
    case ErrorCode::INVALID_MESSAGE_FORMAT:
      return "Invalid message format";
    case ErrorCode::MESSAGE_QUEUE_FULL:
      return "Message queue is full";
    
    // Configuration Errors
    case ErrorCode::CONFIG_FILE_NOT_FOUND:
      return "Configuration file not found";
    case ErrorCode::CONFIG_PARSE_ERROR:
      return "Configuration parsing error";
    case ErrorCode::INVALID_CONFIG_VALUE:
      return "Invalid configuration value";
    case ErrorCode::SPIFFS_INIT_FAILED:
      return "SPIFFS initialization failed";
    
    // State Errors
    case ErrorCode::NOT_INITIALIZED:
      return "Not initialized";
    case ErrorCode::INVALID_STATE:
      return "Invalid state";
    case ErrorCode::OPERATION_NOT_PERMITTED:
      return "Operation not permitted";
    
    // Security Errors
    case ErrorCode::PAIRING_FAILED:
      return "Pairing failed";
    case ErrorCode::AUTHENTICATION_FAILED:
      return "Authentication failed";
    case ErrorCode::ENCRYPTION_FAILED:
      return "Encryption failed";
    case ErrorCode::INVALID_PASSKEY:
      return "Invalid passkey";
    
    // Resource Errors
    case ErrorCode::OUT_OF_MEMORY:
      return "Out of memory";
    case ErrorCode::BUFFER_OVERFLOW:
      return "Buffer overflow";
    case ErrorCode::TIMEOUT:
      return "Operation timeout";
    
    // Unknown
    case ErrorCode::UNKNOWN_ERROR:
    default:
      return "Unknown error";
  }
}

} // namespace BeamErrors

