# Changelog

All notable changes to the BeamLink-ESP32 project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.0] - 2025-10-13

### 🎉 Major Release - Comprehensive Improvements

### Added
- **MTU Negotiation**: Automatic negotiation up to 512 bytes (22x improvement)
  - `getMTU()` method to query current MTU size
  - Messages up to 509 bytes (MTU - 3 byte header)
  
- **Statistics Tracking**: Real-time monitoring and analytics
  - `getMessagesReceived()` - Count of received messages
  - `getMessagesSent()` - Count of sent messages
  - `getErrors()` - Error counter
  - `getUptime()` - System uptime in milliseconds
  - `resetStats()` - Reset all statistics
  
- **Message Parsing Utilities** (`BeamUtils` namespace):
  - `parseCommand()` - Parse "command:action" format
  - `parseCommandValue()` - Parse "command:action:value" format
  - `parseKeyValue()` - Parse "key1=val1,key2=val2" format
  - `split()` - String splitting by delimiter
  - `trim()` - Whitespace trimming
  - `toLower()` / `toUpper()` - Case conversion
  - `startsWith()` / `endsWith()` - String prefix/suffix checking
  - `formatStats()` - Format statistics as string
  - `formatUptime()` - Human-readable uptime formatting
  
- **Error Handling Framework** (`BeamErrors` namespace):
  - Comprehensive error codes categorized by type
  - `toString()` - Convert error codes to messages
  - Error type checking helpers (isInitError, isConnectionError, etc.)
  - `ErrorHandler` callback interface
  - `ErrorInfo` structure for detailed error tracking
  
- **Security Framework** (`BeamSecurity` namespace):
  - Security level options (None, Encryption, Pairing, MITM, SC)
  - Passkey/PIN support structures
  - Bonding configuration
  - Security callbacks interface
  - Foundation for future BLE security implementation
  
- **Unit Testing**:
  - Comprehensive test suite for BeamUtils (40+ tests)
  - PlatformIO test integration
  - Test coverage for all utility functions
  
- **CI/CD Pipeline**:
  - GitHub Actions workflow for automated builds
  - Multi-project build matrix
  - Firmware artifact archiving
  - Code formatting checks
  
- **New Examples**:
  - Sensor Monitor example demonstrating all new features
  - Auto-reporting sensor data
  - Multiple command format support
  - Statistics display commands

### Fixed
- **CRITICAL**: BLE advertising power and interval from config now properly applied
  - Config parameters were loaded but not used in BLE initialization
  - Added power and interval parameters to `begin()` method
  - Proper validation and clamping of values

### Changed
- **API Enhancement**: `begin()` method signature extended
  - Old: `begin(const char* deviceName)`
  - New: `begin(const char* deviceName, int8_t advPowerDbm = 9, uint16_t advIntervalMs = 100)`
  - Backward compatible with default parameters
  
- **Message Size Handling**:
  - Automatic validation against MTU
  - Truncation with warning for oversized messages
  - Error counting for message issues
  
- **Logging Improvements**:
  - Message counters in RX/TX logs: `RX [42]: message`
  - Configuration parameter logging at startup
  - MTU size logged after initialization
  
- **LED Toggle Example**:
  - Updated to use config parameters in begin() call
  - Demonstrates proper configuration usage

### Improved
- **Documentation**:
  - Comprehensive API documentation for all new features
  - Updated README with latest features
  - Detailed IMPROVEMENTS.md with migration guide
  - Example-specific READMEs
  
- **Performance**:
  - 22x larger message capacity (23 → 512 bytes)
  - Optimized BLE advertising intervals
  - Configurable transmission power

### Backward Compatibility
All changes maintain full backward compatibility. Existing code using the old API will continue to work without modifications.

## [1.0.0] - 2025-10-10

### Initial Release

### Added
- Core BLE communication functionality
- NimBLE-based efficient BLE stack
- SPIFFS configuration support
- Auto-reconnect on disconnect
- Message-based callback pattern
- Memory optimization with std::string
- RAII resource management
- Comprehensive error checking
- LED toggle example
- Complete API documentation

### Features
- Simple API: `begin()`, `onMessage()`, `notify()`
- Configuration from `beam.config` file
- Automatic advertising restart
- Connection status tracking
- Device name configuration
- Clean callback pattern

---

## Version History

### Version 2.0.0 Highlights
- 22x larger message capacity
- Real-time statistics tracking
- Built-in message parsing utilities
- Comprehensive error framework
- Unit testing and CI/CD
- Critical bug fixes

### Version 1.0.0 Highlights
- Initial BLE communication framework
- SPIFFS configuration system
- Memory-optimized implementation
- Production-ready error handling

