#pragma once
#include <Arduino.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <variant>

/**
 * @file NexState.h
 * @brief NexState - A Zustand-like state management system for ESP32
 * 
 * NexState provides efficient state management with change detection to avoid
 * unnecessary serial output loops. It tracks state changes and only outputs
 * when values actually change, preventing constant serial.print() calls.
 */

namespace nexstate {

/**
 * @brief Type-safe state value container without dynamic casting
 */
template<typename T>
class StateValue {
public:
    // Default constructor for variant compatibility
    StateValue() : currentValue(T{}), previousValue(T{}), changed(false) {}
    
    explicit StateValue(const T& initialValue) 
        : currentValue(initialValue), previousValue(initialValue), changed(false) {}
    
    void setValue(const T& newValue) {
        if (newValue != currentValue) {
            previousValue = currentValue;
            currentValue = newValue;
            changed = true;
        }
    }
    
    const T& getValue() const { return currentValue; }
    const T& getPreviousValue() const { return previousValue; }
    
    bool hasChanged() const { return changed; }
    
    void markAsRead() { changed = false; }
    
    void reset() {
        previousValue = currentValue;
        changed = false;
    }
    
    std::string toString() const {
        if constexpr (std::is_same_v<T, bool>) {
            return currentValue ? "true" : "false";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, long>) {
            return std::to_string(currentValue);
        } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
            return std::to_string(currentValue);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + currentValue + "\"";
        } else {
            return "unknown";
        }
    }

private:
    T currentValue;
    T previousValue;
    bool changed;
};

/**
 * @brief Device information structure
 */
struct DeviceInfo {
    std::string deviceName;
    std::string deviceId;
    std::string deviceType;
    std::string firmwareVersion;
    int ledPin = 2;
    bool ledActiveHigh = true;
    
    DeviceInfo() = default;
    DeviceInfo(const std::string& name, const std::string& id, 
               const std::string& type, const std::string& fw)
        : deviceName(name), deviceId(id), deviceType(type), firmwareVersion(fw) {}
};

/**
 * @brief Configuration for NexState
 */
struct NexStateConfig {
    bool enableSerialOutput = true;
    bool enableJsonFormat = true;
    bool enableChangeDetection = true;
    unsigned long outputIntervalMs = 1000; // Minimum interval between outputs
    bool outputOnChange = true; // Output immediately when state changes
    bool outputOnInterval = false; // Output periodically regardless of changes
    DeviceInfo deviceInfo; // Device information for output headers
};

/**
 * @brief Main NexState store class
 */
class NexState {
public:
    /**
     * @brief Constructor
     * @param config Configuration for the state store
     */
    explicit NexState(const NexStateConfig& config = NexStateConfig{});
    
    /**
     * @brief Destructor
     */
    ~NexState() = default;
    
    /**
     * @brief Set a state value
     * @param key State key
     * @param value New value
     */
    template<typename T>
    void set(const std::string& key, const T& value) {
        auto it = stateValues.find(key);
        if (it != stateValues.end()) {
            // Check if the existing value is of the same type
            auto existingValue = std::get_if<StateValue<T>>(&it->second);
            if (existingValue) {
                existingValue->setValue(value);
            } else {
                // Type mismatch, replace with new value
                it->second = StateValue<T>(value);
            }
        } else {
            // Insert new value
            stateValues.emplace(key, StateValue<T>(value));
        }
        
        if (config.outputOnChange) {
            checkAndOutput();
        }
    }
    
    /**
     * @brief Get a state value
     * @param key State key
     * @param defaultValue Default value if key doesn't exist
     * @return Current value
     */
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T{}) const {
        auto it = stateValues.find(key);
        if (it != stateValues.end()) {
            auto value = std::get_if<StateValue<T>>(&it->second);
            if (value) {
                return value->getValue();
            }
        }
        return defaultValue;
    }
    
    /**
     * @brief Check if a state value has changed
     * @param key State key
     * @return true if the value has changed since last check
     */
    template<typename T>
    bool hasChanged(const std::string& key) const {
        auto it = stateValues.find(key);
        if (it != stateValues.end()) {
            auto value = std::get_if<StateValue<T>>(&it->second);
            if (value) {
                return value->hasChanged();
            }
        }
        return false;
    }
    
    /**
     * @brief Mark a state value as read (clear change flag)
     * @param key State key
     */
    template<typename T>
    void markAsRead(const std::string& key) {
        auto it = stateValues.find(key);
        if (it != stateValues.end()) {
            auto value = std::get_if<StateValue<T>>(&it->second);
            if (value) {
                value->markAsRead();
            }
        }
    }
    
    /**
     * @brief Get all changed state keys
     * @return Vector of keys that have changed
     */
    std::vector<std::string> getChangedKeys() const {
        std::vector<std::string> changed;
        for (const auto& pair : stateValues) {
            // Check each possible type for changes
            if (std::visit([](const auto& value) { return value.hasChanged(); }, pair.second)) {
                changed.push_back(pair.first);
            }
        }
        return changed;
    }
    
    /**
     * @brief Check if any state has changed
     * @return true if any state value has changed
     */
    bool hasAnyChanged() const {
        for (const auto& pair : stateValues) {
            if (std::visit([](const auto& value) { return value.hasChanged(); }, pair.second)) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * @brief Mark all states as read
     */
    void markAllAsRead() {
        for (auto& pair : stateValues) {
            std::visit([](auto& value) { value.markAsRead(); }, pair.second);
        }
    }
    
    /**
     * @brief Update the state store (call this in loop())
     */
    void update() {
        unsigned long now = millis();
        
        if (config.outputOnInterval && (now - lastOutputTime >= config.outputIntervalMs)) {
            outputState();
            lastOutputTime = now;
        }
    }
    
    /**
     * @brief Force output of current state
     */
    void outputState() {
        if (!config.enableSerialOutput) return;
        
        if (config.enableJsonFormat) {
            outputJsonState();
        } else {
            outputTextState();
        }
        
        markAllAsRead();
        lastOutputTime = millis();
    }
    
    /**
     * @brief Get state as JSON string with device info
     * @return JSON representation of current state with device info
     */
    std::string getStateAsJson() const {
        std::string json = "{";
        json += "\"device\":\"" + config.deviceInfo.deviceName + "\",";
        json += "\"id\":\"" + config.deviceInfo.deviceId + "\",";
        json += "\"type\":\"" + config.deviceInfo.deviceType + "\",";
        json += "\"fw\":\"" + config.deviceInfo.firmwareVersion + "\",";
        json += "\"state\":{";
        
        bool first = true;
        for (const auto& pair : stateValues) {
            if (!first) json += ",";
            json += "\"" + pair.first + "\":";
            std::visit([&json](const auto& value) { json += value.toString(); }, pair.second);
            first = false;
        }
        
        json += "}}";
        return json;
    }
    
    /**
     * @brief Get state as text string with device info
     * @return Text representation of current state with device info
     */
    std::string getStateAsText() const {
        std::string text = "Device: " + config.deviceInfo.deviceName;
        text += " (ID: " + config.deviceInfo.deviceId;
        text += ", Type: " + config.deviceInfo.deviceType;
        text += ", FW: " + config.deviceInfo.firmwareVersion + ")";
        text += " | State: ";
        
        bool first = true;
        for (const auto& pair : stateValues) {
            if (!first) text += ", ";
            text += pair.first + "=";
            std::visit([&text](const auto& value) { 
                std::string val = value.toString();
                // Remove quotes for text format
                if (val.front() == '"' && val.back() == '"') {
                    val = val.substr(1, val.length() - 2);
                }
                text += val;
            }, pair.second);
            first = false;
        }
        
        return text;
    }
    
    /**
     * @brief Subscribe to state changes
     * @param callback Function to call when state changes
     */
    void subscribe(std::function<void(const std::string&, const std::string&)> callback) {
        changeCallback = callback;
    }
    
    /**
     * @brief Clear all state
     */
    void clear() {
        stateValues.clear();
    }
    
    /**
     * @brief Get number of state values
     * @return Count of state values
     */
    size_t size() const { return stateValues.size(); }

private:
    NexStateConfig config;
    
    // Type-safe variant storage for different state value types
    using StateValueVariant = std::variant<
        StateValue<bool>,
        StateValue<int>,
        StateValue<float>,
        StateValue<std::string>
    >;
    
    std::unordered_map<std::string, StateValueVariant> stateValues;
    std::function<void(const std::string&, const std::string&)> changeCallback;
    unsigned long lastOutputTime = 0;
    
    void checkAndOutput() {
        if (config.enableChangeDetection && hasAnyChanged()) {
            outputState();
        }
    }
    
    void outputJsonState() {
        Serial.println(getStateAsJson().c_str());
    }
    
    void outputTextState() {
        Serial.println(getStateAsText().c_str());
    }
};

/**
 * @brief Global NexState instance
 */
extern std::unique_ptr<NexState> g_nexState;

/**
 * @brief Initialize NexState with configuration
 * @param config Configuration for NexState
 * @return true if initialization successful
 */
bool initialize(const NexStateConfig& config = NexStateConfig{});

/**
 * @brief Get the global NexState instance
 * @return Reference to global NexState instance
 */
NexState& State();

/**
 * @brief Update NexState (call in loop())
 */
void update();

/**
 * @brief Shutdown NexState
 */
void shutdown();

} // namespace nexstate
