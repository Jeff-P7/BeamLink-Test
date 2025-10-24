#pragma once
#include <string>
#include <Arduino.h>

/**
 * @file BeamErrors.h
 * @brief Error codes and error handling for BeamLink
 * 
 * This file defines comprehensive error codes for BeamLink operations
 * and provides utilities for error handling and reporting.
 */

namespace BeamErrors {

  /**
   * @enum ErrorCode
   * @brief Comprehensive error codes for BeamLink operations
   */
  enum class ErrorCode {
    // Success
    OK = 0,
    
    // Initialization Errors (1-99)
    ALREADY_INITIALIZED = 1,
    INVALID_DEVICE_NAME = 2,
    BLE_INIT_FAILED = 3,
    SERVER_CREATE_FAILED = 4,
    SERVICE_CREATE_FAILED = 5,
    CHARACTERISTIC_CREATE_FAILED = 6,
    ADVERTISING_FAILED = 7,
    
    // Connection Errors (100-199)
    NOT_CONNECTED = 100,
    CONNECTION_LOST = 101,
    DISCONNECTED = 102,
    
    // Message Errors (200-299)
    MESSAGE_TOO_LARGE = 200,
    MESSAGE_EMPTY = 201,
    NOTIFY_FAILED = 202,
    INVALID_MESSAGE_FORMAT = 203,
    MESSAGE_QUEUE_FULL = 204,
    
    // Configuration Errors (300-399)
    CONFIG_FILE_NOT_FOUND = 300,
    CONFIG_PARSE_ERROR = 301,
    INVALID_CONFIG_VALUE = 302,
    SPIFFS_INIT_FAILED = 303,
    
    // State Errors (400-499)
    NOT_INITIALIZED = 400,
    INVALID_STATE = 401,
    OPERATION_NOT_PERMITTED = 402,
    
    // Security Errors (500-599)
    PAIRING_FAILED = 500,
    AUTHENTICATION_FAILED = 501,
    ENCRYPTION_FAILED = 502,
    INVALID_PASSKEY = 503,
    
    // Resource Errors (600-699)
    OUT_OF_MEMORY = 600,
    BUFFER_OVERFLOW = 601,
    TIMEOUT = 602,
    
    // Unknown/Other
    UNKNOWN_ERROR = 999
  };

  /**
   * @brief Convert error code to human-readable string
   * 
   * @param code Error code
   * @return String description of the error
   */
  const char* toString(ErrorCode code);

  /**
   * @brief Check if error code represents a success
   * 
   * @param code Error code
   * @return true if code is OK
   */
  inline bool isOk(ErrorCode code) {
    return code == ErrorCode::OK;
  }

  /**
   * @brief Check if error code represents a failure
   * 
   * @param code Error code
   * @return true if code is not OK
   */
  inline bool isError(ErrorCode code) {
    return code != ErrorCode::OK;
  }

  /**
   * @brief Check if error is initialization-related
   * 
   * @param code Error code
   * @return true if error is in initialization range
   */
  inline bool isInitError(ErrorCode code) {
    int c = static_cast<int>(code);
    return c >= 1 && c < 100;
  }

  /**
   * @brief Check if error is connection-related
   * 
   * @param code Error code
   * @return true if error is in connection range
   */
  inline bool isConnectionError(ErrorCode code) {
    int c = static_cast<int>(code);
    return c >= 100 && c < 200;
  }

  /**
   * @brief Check if error is message-related
   * 
   * @param code Error code
   * @return true if error is in message range
   */
  inline bool isMessageError(ErrorCode code) {
    int c = static_cast<int>(code);
    return c >= 200 && c < 300;
  }

  /**
   * @class ErrorHandler
   * @brief Error handler callback interface
   * 
   * Implement this class to receive error notifications
   */
  class ErrorHandler {
  public:
    virtual ~ErrorHandler() = default;
    
    /**
     * @brief Called when an error occurs
     * 
     * @param code Error code
     * @param message Additional error message
     */
    virtual void onError(ErrorCode code, const std::string& message) = 0;
  };

  /**
   * @struct ErrorInfo
   * @brief Detailed error information
   */
  struct ErrorInfo {
    ErrorCode code;
    std::string message;
    unsigned long timestamp;
    
    ErrorInfo(ErrorCode c, const std::string& msg) 
      : code(c), message(msg), timestamp(millis()) {}
  };

} // namespace BeamErrors

