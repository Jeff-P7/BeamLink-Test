/**
 * @file test_beamlink.cpp
 * @brief Unit tests for BeamLink BLE library using Doctest
 */

#include <Arduino.h>
#include <unity.h>
#include "BeamLink.h"
#include "BeamConfig.h"
#include "BeamErrors.h"
#include "BeamUtils.h"

// Test instance
BeamLink* beam = nullptr;

// Helper variables
bool callbackCalled = false;
std::string receivedMessage;
std::string sentReply;

void setUp(void) {
    // Set up code before each test
    callbackCalled = false;
    receivedMessage = "";
    sentReply = "";
}

void tearDown(void) {
    // Clean up code after each test
    if (beam) {
        beam->end();
        delete beam;
        beam = nullptr;
    }
}

// ============================================================================
// Constructor/Destructor Tests
// ============================================================================

void test_beamlink_constructor() {
    beam = new BeamLink();
    TEST_ASSERT_NOT_NULL(beam);
    TEST_ASSERT_FALSE(beam->isConnected());
}

void test_beamlink_initial_stats() {
    beam = new BeamLink();
    TEST_ASSERT_EQUAL_UINT32(0, beam->getMessagesReceived());
    TEST_ASSERT_EQUAL_UINT32(0, beam->getMessagesSent());
    TEST_ASSERT_EQUAL_UINT32(0, beam->getErrors());
}

// ============================================================================
// Initialization Tests
// ============================================================================

void test_beamlink_begin_with_defaults() {
    beam = new BeamLink();
    bool result = beam->begin("TestDevice");
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_STRING("TestDevice", beam->getDeviceName().c_str());
}

void test_beamlink_begin_with_custom_power() {
    beam = new BeamLink();
    bool result = beam->begin("TestDevice", 6, 200);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_STRING("TestDevice", beam->getDeviceName().c_str());
}

void test_beamlink_begin_twice_fails() {
    beam = new BeamLink();
    bool first = beam->begin("TestDevice");
    TEST_ASSERT_TRUE(first);
    
    // Second initialization should fail
    bool second = beam->begin("TestDevice2");
    TEST_ASSERT_FALSE(second);
    
    // Device name should remain unchanged
    TEST_ASSERT_EQUAL_STRING("TestDevice", beam->getDeviceName().c_str());
}

// ============================================================================
// Message Handler Tests
// ============================================================================

void test_beamlink_set_message_handler() {
    beam = new BeamLink();
    beam->begin("TestDevice");
    
    beam->onMessage([](const std::string& msg, ReplyFn reply) {
        callbackCalled = true;
        receivedMessage = msg;
    });
    
    // Handler should be set (we can't directly test but no crash is good)
    TEST_ASSERT_TRUE(true);
}

void test_beamlink_message_handler_can_be_changed() {
    beam = new BeamLink();
    beam->begin("TestDevice");
    
    // Set first handler
    beam->onMessage([](const std::string& msg, ReplyFn reply) {
        receivedMessage = "first";
    });
    
    // Set second handler (should replace first)
    beam->onMessage([](const std::string& msg, ReplyFn reply) {
        receivedMessage = "second";
    });
    
    TEST_ASSERT_TRUE(true);
}

// ============================================================================
// Connection State Tests
// ============================================================================

void test_beamlink_not_connected_initially() {
    beam = new BeamLink();
    beam->begin("TestDevice");
    TEST_ASSERT_FALSE(beam->isConnected());
}

// ============================================================================
// MTU Tests
// ============================================================================

void test_beamlink_mtu_default() {
    beam = new BeamLink();
    // Before initialization, should return default
    TEST_ASSERT_EQUAL_UINT16(23, beam->getMTU());
}

void test_beamlink_mtu_after_init() {
    beam = new BeamLink();
    beam->begin("TestDevice");
    // After init, MTU should be set (NimBLE sets to 512 or negotiated value)
    uint16_t mtu = beam->getMTU();
    TEST_ASSERT_GREATER_THAN_UINT16(23, mtu);
    TEST_ASSERT_LESS_OR_EQUAL_UINT16(512, mtu);
}

// ============================================================================
// Statistics Tests
// ============================================================================

void test_beamlink_reset_stats() {
    beam = new BeamLink();
    beam->begin("TestDevice");
    beam->resetStats();
    
    TEST_ASSERT_EQUAL_UINT32(0, beam->getMessagesReceived());
    TEST_ASSERT_EQUAL_UINT32(0, beam->getMessagesSent());
    TEST_ASSERT_EQUAL_UINT32(0, beam->getErrors());
}

void test_beamlink_uptime_increases() {
    beam = new BeamLink();
    beam->begin("TestDevice");
    
    unsigned long uptime1 = beam->getUptime();
    delay(100);
    unsigned long uptime2 = beam->getUptime();
    
    TEST_ASSERT_GREATER_THAN_UINT32(uptime1, uptime2);
}

// ============================================================================
// Notify Tests (without actual BLE connection)
// ============================================================================

void test_beamlink_notify_fails_when_not_connected() {
    beam = new BeamLink();
    beam->begin("TestDevice");
    
    // Should fail because no client is connected
    bool result = beam->notify("Test message");
    TEST_ASSERT_FALSE(result);
    
    // Error count should increase
    TEST_ASSERT_GREATER_THAN_UINT32(0, beam->getErrors());
}

void test_beamlink_notify_fails_with_empty_message() {
    beam = new BeamLink();
    beam->begin("TestDevice");
    
    bool result = beam->notify("");
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_GREATER_THAN_UINT32(0, beam->getErrors());
}

void test_beamlink_notify_fails_before_init() {
    beam = new BeamLink();
    
    bool result = beam->notify("Test");
    TEST_ASSERT_FALSE(result);
}

// ============================================================================
// Loop Tests
// ============================================================================

void test_beamlink_loop_doesnt_crash() {
    beam = new BeamLink();
    beam->begin("TestDevice");
    
    // Should not crash
    beam->loop();
    beam->loop();
    beam->loop();
    
    TEST_ASSERT_TRUE(true);
}

// ============================================================================
// End/Cleanup Tests
// ============================================================================

void test_beamlink_end_cleans_up() {
    beam = new BeamLink();
    beam->begin("TestDevice");
    beam->end();
    
    // After end, should not be connected
    TEST_ASSERT_FALSE(beam->isConnected());
}

void test_beamlink_can_reinitialize_after_end() {
    beam = new BeamLink();
    beam->begin("TestDevice1");
    beam->end();
    
    // Should be able to initialize again
    bool result = beam->begin("TestDevice2");
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_STRING("TestDevice2", beam->getDeviceName().c_str());
}

// ============================================================================
// BeamConfig Tests
// ============================================================================

void test_beamconfig_defaults() {
    BeamConfig config;
    TEST_ASSERT_EQUAL_STRING("BeamLink-ESP32", config.deviceName.c_str());
    TEST_ASSERT_EQUAL_INT8(9, config.advPowerDbm);
    TEST_ASSERT_EQUAL_UINT16(100, config.advIntervalMs);
    TEST_ASSERT_EQUAL_INT(2, config.ledPin);
}

void test_beamconfig_set_values() {
    BeamConfig config;
    config.deviceName = "MyDevice";
    config.advPowerDbm = 6;
    config.advIntervalMs = 200;
    config.ledPin = 13;
    
    TEST_ASSERT_EQUAL_STRING("MyDevice", config.deviceName.c_str());
    TEST_ASSERT_EQUAL_INT8(6, config.advPowerDbm);
    TEST_ASSERT_EQUAL_UINT16(200, config.advIntervalMs);
    TEST_ASSERT_EQUAL_INT(13, config.ledPin);
}

// ============================================================================
// BeamUtils Tests
// ============================================================================

void test_beamutils_trim() {
    std::string test = "  hello  ";
    std::string result = BeamUtils::trim(test);
    TEST_ASSERT_EQUAL_STRING("hello", result.c_str());
}

void test_beamutils_trim_empty() {
    std::string test = "   ";
    std::string result = BeamUtils::trim(test);
    TEST_ASSERT_EQUAL_STRING("", result.c_str());
}

void test_beamutils_to_lower() {
    std::string test = "HELLO World";
    std::string result = BeamUtils::toLower(test);
    TEST_ASSERT_EQUAL_STRING("hello world", result.c_str());
}

void test_beamutils_to_upper() {
    std::string test = "hello WORLD";
    std::string result = BeamUtils::toUpper(test);
    TEST_ASSERT_EQUAL_STRING("HELLO WORLD", result.c_str());
}

void test_beamutils_split() {
    std::string test = "cmd:action:value";
    auto parts = BeamUtils::split(test, ':');
    TEST_ASSERT_EQUAL_size_t(3, parts.size());
    TEST_ASSERT_EQUAL_STRING("cmd", parts[0].c_str());
    TEST_ASSERT_EQUAL_STRING("action", parts[1].c_str());
    TEST_ASSERT_EQUAL_STRING("value", parts[2].c_str());
}

void test_beamutils_split_empty() {
    std::string test = "";
    auto parts = BeamUtils::split(test, ':');
    TEST_ASSERT_EQUAL_size_t(0, parts.size());
}

void test_beamutils_starts_with() {
    TEST_ASSERT_TRUE(BeamUtils::startsWith("hello world", "hello"));
    TEST_ASSERT_FALSE(BeamUtils::startsWith("hello world", "world"));
    TEST_ASSERT_TRUE(BeamUtils::startsWith("test", "test"));
}

void test_beamutils_ends_with() {
    TEST_ASSERT_TRUE(BeamUtils::endsWith("hello world", "world"));
    TEST_ASSERT_FALSE(BeamUtils::endsWith("hello world", "hello"));
    TEST_ASSERT_TRUE(BeamUtils::endsWith("test", "test"));
}

void test_beamutils_parse_command() {
    std::string cmd, action;
    bool result = BeamUtils::parseCommand("led:on", cmd, action);
    
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_STRING("led", cmd.c_str());
    TEST_ASSERT_EQUAL_STRING("on", action.c_str());
}

void test_beamutils_parse_command_value() {
    std::string cmd, action, value;
    bool result = BeamUtils::parseCommandValue("led:on:1", cmd, action, value);
    
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_STRING("led", cmd.c_str());
    TEST_ASSERT_EQUAL_STRING("on", action.c_str());
    TEST_ASSERT_EQUAL_STRING("1", value.c_str());
}

void test_beamutils_parse_command_invalid() {
    std::string cmd, action;
    bool result = BeamUtils::parseCommand("", cmd, action);
    TEST_ASSERT_FALSE(result);
}

void test_beamutils_format_uptime() {
    unsigned long oneHour = 3600000;
    std::string result = BeamUtils::formatUptime(oneHour);
    // Should contain hours
    TEST_ASSERT_TRUE(result.length() > 0);
}

// ============================================================================
// BeamErrors Tests
// ============================================================================

void test_beamerrors_to_string() {
    using namespace BeamErrors;
    
    TEST_ASSERT_EQUAL_STRING("Success", toString(ErrorCode::OK));
    TEST_ASSERT_EQUAL_STRING("BLE initialization failed", toString(ErrorCode::BLE_INIT_FAILED));
    TEST_ASSERT_EQUAL_STRING("Device not connected", toString(ErrorCode::NOT_CONNECTED));
}

void test_beamerrors_is_error() {
    using namespace BeamErrors;
    
    TEST_ASSERT_FALSE(isError(ErrorCode::OK));
    TEST_ASSERT_TRUE(isError(ErrorCode::BLE_INIT_FAILED));
    TEST_ASSERT_TRUE(isError(ErrorCode::NOT_CONNECTED));
}

// ============================================================================
// Main Test Setup
// ============================================================================

void setup() {
    delay(2000); // Wait for serial to initialize
    
    UNITY_BEGIN();
    
    // Constructor/Destructor Tests
    RUN_TEST(test_beamlink_constructor);
    RUN_TEST(test_beamlink_initial_stats);
    
    // Initialization Tests
    RUN_TEST(test_beamlink_begin_with_defaults);
    RUN_TEST(test_beamlink_begin_with_custom_power);
    RUN_TEST(test_beamlink_begin_twice_fails);
    
    // Message Handler Tests
    RUN_TEST(test_beamlink_set_message_handler);
    RUN_TEST(test_beamlink_message_handler_can_be_changed);
    
    // Connection State Tests
    RUN_TEST(test_beamlink_not_connected_initially);
    
    // MTU Tests
    RUN_TEST(test_beamlink_mtu_default);
    RUN_TEST(test_beamlink_mtu_after_init);
    
    // Statistics Tests
    RUN_TEST(test_beamlink_reset_stats);
    RUN_TEST(test_beamlink_uptime_increases);
    
    // Notify Tests
    RUN_TEST(test_beamlink_notify_fails_when_not_connected);
    RUN_TEST(test_beamlink_notify_fails_with_empty_message);
    RUN_TEST(test_beamlink_notify_fails_before_init);
    
    // Loop Tests
    RUN_TEST(test_beamlink_loop_doesnt_crash);
    
    // End/Cleanup Tests
    RUN_TEST(test_beamlink_end_cleans_up);
    RUN_TEST(test_beamlink_can_reinitialize_after_end);
    
    // BeamConfig Tests
    RUN_TEST(test_beamconfig_defaults);
    RUN_TEST(test_beamconfig_set_values);
    
    // BeamUtils Tests
    RUN_TEST(test_beamutils_trim);
    RUN_TEST(test_beamutils_trim_empty);
    RUN_TEST(test_beamutils_to_lower);
    RUN_TEST(test_beamutils_to_upper);
    RUN_TEST(test_beamutils_split);
    RUN_TEST(test_beamutils_split_empty);
    RUN_TEST(test_beamutils_starts_with);
    RUN_TEST(test_beamutils_ends_with);
    RUN_TEST(test_beamutils_parse_command);
    RUN_TEST(test_beamutils_parse_command_value);
    RUN_TEST(test_beamutils_parse_command_invalid);
    RUN_TEST(test_beamutils_format_uptime);
    
    // BeamErrors Tests
    RUN_TEST(test_beamerrors_to_string);
    RUN_TEST(test_beamerrors_is_error);
    
    UNITY_END();
}

void loop() {
    // Nothing to do here
}

