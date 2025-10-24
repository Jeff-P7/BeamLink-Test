#pragma once
#include <NimBLEDevice.h>

/**
 * @file BeamSecurity.h
 * @brief BLE Security and pairing features for BeamLink
 * 
 * This file provides optional security features for BeamLink including
 * pairing, bonding, and encryption capabilities.
 * 
 * @note Security features are optional and can be enabled based on your
 *       security requirements. Most casual IoT applications work fine
 *       without pairing.
 */

namespace BeamSecurity {

  /**
   * @enum SecurityLevel
   * @brief Security level options for BLE connections
   */
  enum class SecurityLevel {
    NONE = 0,           ///< No security (default)
    ENCRYPTION,         ///< Encrypted connection (no pairing)
    PAIRING,            ///< Pairing required (Just Works)
    PAIRING_MITM,       ///< Pairing with PIN/Passkey (MITM protection)
    PAIRING_MITM_SC     ///< Secure Connections with MITM
  };

  /**
   * @struct SecurityConfig
   * @brief Security configuration structure
   */
  struct SecurityConfig {
    SecurityLevel level = SecurityLevel::NONE;
    uint32_t passkey = 0;           ///< 6-digit PIN (000000-999999)
    bool enableBonding = false;     ///< Store pairing for reconnection
    bool requireMITM = false;       ///< Require Man-in-the-Middle protection
    bool secureConnections = true;  ///< Use BLE 4.2 Secure Connections
  };

  /**
   * @brief Configure BLE security settings
   * 
   * @param config Security configuration structure
   * @return true if configuration was successful
   * 
   * @example
   * ```cpp
   * BeamSecurity::SecurityConfig sec;
   * sec.level = BeamSecurity::SecurityLevel::PAIRING_MITM;
   * sec.passkey = 123456;
   * sec.enableBonding = true;
   * BeamSecurity::configureSecurity(sec);
   * ```
   */
  bool configureSecurity(const SecurityConfig& config);

  /**
   * @brief Set a static passkey for pairing
   * 
   * @param passkey 6-digit PIN (000000-999999)
   * @return true if passkey was set successfully
   * 
   * @note This should be called before starting BLE advertising
   */
  bool setPasskey(uint32_t passkey);

  /**
   * @brief Enable/disable bonding
   * 
   * When bonding is enabled, paired devices are remembered and can
   * reconnect without re-pairing.
   * 
   * @param enable true to enable bonding, false to disable
   */
  void enableBonding(bool enable);

  /**
   * @brief Clear all bonded devices
   * 
   * Removes all stored bonding information. Devices will need to
   * re-pair on next connection.
   */
  void clearBonds();

  /**
   * @brief Get number of bonded devices
   * 
   * @return Count of bonded devices
   */
  int getBondedDeviceCount();

  /**
   * @brief Check if a device is currently paired
   * 
   * @return true if a device is paired
   */
  bool isPaired();

  /**
   * @brief Security callback interface
   * 
   * Implement this class to receive security-related callbacks
   */
  class SecurityCallbacks {
  public:
    virtual ~SecurityCallbacks() = default;
    
    /**
     * @brief Called when pairing is requested
     * 
     * @param passkey The passkey to display to user (if display method)
     */
    virtual void onPairingRequest(uint32_t passkey) {}
    
    /**
     * @brief Called when pairing is completed
     * 
     * @param success true if pairing was successful
     */
    virtual void onPairingComplete(bool success) {}
    
    /**
     * @brief Called when passkey needs to be confirmed
     * 
     * @param passkey The passkey to confirm
     * @return true to confirm, false to reject
     */
    virtual bool onConfirmPasskey(uint32_t passkey) { return true; }
    
    /**
     * @brief Called when authentication is complete
     * 
     * @param success true if authentication was successful
     */
    virtual void onAuthenticationComplete(bool success) {}
  };

  /**
   * @brief Set security callbacks
   * 
   * @param callbacks Pointer to SecurityCallbacks implementation
   */
  void setSecurityCallbacks(SecurityCallbacks* callbacks);

} // namespace BeamSecurity

