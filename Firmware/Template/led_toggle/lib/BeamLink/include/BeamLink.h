#pragma once
#include <NimBLEDevice.h>
#include <functional>
#include <Arduino.h>
#include <memory>

/**
 * @file BeamLink.h
 * @brief BLE communication library for ESP32
 * 
 * BeamLink provides a simple BLE communication interface for ESP32 devices.
 * It allows bidirectional communication with BLE clients through a custom
 * service with TX (notify) and RX (write) characteristics.
 */

/**
 * @brief Function type for sending replies to clients
 * 
 * This function type is used by the message handler to send responses
 * back to the connected BLE client.
 */
using ReplyFn = std::function<void(const std::string&)>;

/**
 * @brief Function type for handling incoming messages
 * 
 * This function type is called when a message is received from a BLE client.
 * The first parameter is the received message, and the second is a function
 * to send a reply back to the client.
 */
using MessageHandler = std::function<void(const std::string& in, ReplyFn reply)>;

/**
 * @class BeamLink
 * @brief Main BLE communication class
 * 
 * BeamLink provides a simple interface for BLE communication on ESP32.
 * It creates a BLE server with a custom service that supports bidirectional
 * communication through TX (notify) and RX (write) characteristics.
 * 
 * @example
 * ```cpp
 * BeamLink beam;
 * 
 * void setup() {
 *   if (beam.begin("MyDevice")) {
 *     beam.onMessage([](const std::string& msg, auto reply) {
 *       reply("Echo: " + msg);
 *     });
 *   }
 * }
 * 
 * void loop() {
 *   beam.loop();
 * }
 * ```
 */
class BeamLink {
public:
  /**
   * @brief Constructor
   * 
   * Creates a new BeamLink instance. The instance must be initialized
   * with begin() before use.
   */
  BeamLink();
  
  /**
   * @brief Destructor
   * 
   * Automatically cleans up resources when the object is destroyed.
   */
  ~BeamLink();

  /**
   * @brief Initialize BLE and start advertising
   * 
   * Initializes the BLE device, creates the server, service, and characteristic,
   * and starts advertising for client connections.
   * 
   * @param deviceName The name to advertise as (max 32 characters)
   * @param advPowerDbm Advertising power in dBm (-12 to +9, default: 9)
   * @param advIntervalMs Advertising interval in ms (20 to 10240, default: 100)
   * @param serviceUuid BLE Service UUID (default: from Uuids.h)
   * @param characteristicUuid BLE Characteristic UUID (default: from Uuids.h)
   * @return true if initialization was successful, false otherwise
   * 
   * @note This function can only be called once per instance. Call end() to
   *       reset the instance if needed.
   */
  bool begin(const char* deviceName, int8_t advPowerDbm = 9, uint16_t advIntervalMs = 100,
             const char* serviceUuid = nullptr, const char* characteristicUuid = nullptr);

  /**
   * @brief Register message handler for incoming messages
   * 
   * Sets the function to be called when a message is received from a BLE client.
   * The handler receives the message and a reply function to send responses.
   * 
   * @param handler Function to handle incoming messages
   * 
   * @example
   * ```cpp
   * beam.onMessage([](const std::string& msg, auto reply) {
   *   if (msg == "ping") {
   *     reply("pong");
   *   }
   * });
   * ```
   */
  void onMessage(MessageHandler handler);

  /**
   * @brief Send a message to the connected client
   * 
   * Sends a message to the currently connected BLE client via the TX characteristic.
   * The message is sent as a notification.
   * 
   * @param msg The message to send
   * @return true if the message was sent successfully, false otherwise
   * 
   * @note This function will fail if no client is connected or if the message is empty.
   */
  bool notify(const std::string& msg);

  /**
   * @brief Check if a client is connected
   * 
   * @return true if a BLE client is currently connected, false otherwise
   */
  bool isConnected() const { return deviceConnected; }

  /**
   * @brief Get the device name
   * 
   * @return The device name that was set during initialization
   */
  const std::string& getDeviceName() const { return deviceName; }

  /**
   * @brief Get the MTU (Maximum Transmission Unit) size
   * 
   * @return Current MTU size in bytes (default: 23, max: 512)
   */
  uint16_t getMTU() const;

  /**
   * @brief Get number of messages received
   * 
   * @return Count of messages received since initialization
   */
  uint32_t getMessagesReceived() const { return messagesReceived; }

  /**
   * @brief Get number of messages sent
   * 
   * @return Count of messages sent since initialization
   */
  uint32_t getMessagesSent() const { return messagesSent; }

  /**
   * @brief Get number of errors encountered
   * 
   * @return Count of errors since initialization
   */
  uint32_t getErrors() const { return errorCount; }

  /**
   * @brief Get uptime in milliseconds
   * 
   * @return Milliseconds since begin() was called
   */
  unsigned long getUptime() const;

  /**
   * @brief Reset statistics counters
   */
  void resetStats();

  /**
   * @brief Main loop function
   * 
   * This function should be called regularly in the main loop() function.
   * It handles BLE operations and can be extended for future features like
   * timers, heartbeats, etc.
   * 
   * @note This function includes a small delay for proper BLE operation.
   */
  void loop();

  /**
   * @brief Cleanup resources and stop BLE
   * 
   * Stops advertising, disconnects clients, and cleans up BLE resources.
   * The instance can be reinitialized with begin() after calling end().
   */
  void end();

private:
  // Forward declarations for callback classes
  class ServerCallbacks;
  class RxCallbacks;
  
  // BLE objects
  NimBLEServer* pServer = nullptr;         ///< BLE server instance
  NimBLECharacteristic* pChar = nullptr;  ///< Main characteristic (read/write/notify)
  
  // State
  MessageHandler messageHandler = nullptr; ///< Message handler function
  bool deviceConnected = false;            ///< Client connection status
  bool initialized = false;                ///< Initialization status
  std::string deviceName;                  ///< Device name
  std::string serviceUuid;                 ///< BLE Service UUID
  std::string characteristicUuid;          ///< BLE Characteristic UUID
  
  // Statistics
  uint32_t messagesReceived = 0;           ///< Count of messages received
  uint32_t messagesSent = 0;               ///< Count of messages sent
  uint32_t errorCount = 0;                 ///< Count of errors
  unsigned long startTime = 0;             ///< Start time for uptime calculation
  
  // Callback objects
  std::unique_ptr<ServerCallbacks> serverCallbacks; ///< Server callbacks
  std::unique_ptr<RxCallbacks> rxCallbacks;         ///< RX callbacks
  
  // Helper methods
  bool setupService();                      ///< Setup BLE service and characteristics
  bool startAdvertising(uint16_t intervalMs); ///< Start BLE advertising with interval
};
