# BeamLink Tests

This directory contains unit tests for the BeamLink library using the Unity test framework.

## Test Files

- **test_beamlink.cpp** - Comprehensive tests for all BeamLink functionality
- **test_beamutils.cpp** - Additional utility function tests

## Running Tests

### Run all tests
```bash
pio test
```

### Run tests on specific environment
```bash
pio test -e esp32dev
```

### Run specific test file
```bash
pio test -f test_beamlink
```

### Run with verbose output
```bash
pio test -v
```

## Test Coverage

### BeamLink Core Tests
- ✅ Constructor and destructor
- ✅ Initialization with default and custom parameters
- ✅ Preventing double initialization
- ✅ Message handler registration
- ✅ Connection state management
- ✅ MTU handling
- ✅ Statistics tracking (messages sent/received, errors)
- ✅ Uptime calculation
- ✅ Notify functionality validation
- ✅ Loop execution
- ✅ Cleanup and reinitialization

### BeamConfig Tests
- ✅ Default configuration values
- ✅ Custom configuration settings
- ✅ Configuration validation

### BeamUtils Tests
- ✅ String trimming
- ✅ Case conversion (toUpper/toLower)
- ✅ String splitting
- ✅ String prefix/suffix checking
- ✅ String replacement
- ✅ Command parsing

### BeamErrors Tests
- ✅ Error code to string conversion
- ✅ Error validation

## Test Framework

These tests use the **Unity** test framework, which is integrated with PlatformIO. Unity provides:

- Simple assertion macros
- Automatic test discovery
- Clear test output
- Support for embedded systems
- setUp/tearDown hooks for each test

## Writing New Tests

To add new tests:

1. Create a new test function following the naming convention `test_<module>_<functionality>`
2. Use Unity assertion macros (see list below)
3. Add the test to the `setup()` function using `RUN_TEST()`
4. Implement `setUp()` and `tearDown()` if needed for test isolation

### Common Unity Assertions

```cpp
TEST_ASSERT_TRUE(condition)
TEST_ASSERT_FALSE(condition)
TEST_ASSERT_EQUAL(expected, actual)
TEST_ASSERT_EQUAL_STRING(expected, actual)
TEST_ASSERT_EQUAL_UINT32(expected, actual)
TEST_ASSERT_NOT_NULL(pointer)
TEST_ASSERT_NULL(pointer)
TEST_ASSERT_GREATER_THAN(threshold, actual)
TEST_ASSERT_LESS_THAN(threshold, actual)
```

## Example Test

```cpp
void test_example_feature() {
    BeamLink beam;
    bool result = beam.begin("TestDevice");
    
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_STRING("TestDevice", beam.getDeviceName().c_str());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_example_feature);
    UNITY_END();
}
```

## Notes

- Tests require an ESP32 board to be connected for hardware-dependent features
- Some BLE functionality tests are limited without an actual BLE connection
- Tests automatically clean up resources using `tearDown()`
- Each test runs in isolation with fresh BeamLink instances

## Continuous Integration

These tests can be integrated into CI/CD pipelines:

```yaml
# Example GitHub Actions
- name: Run Tests
  run: pio test --environment esp32dev
```

## Troubleshooting

### Tests Won't Compile
- Ensure C++17 is enabled in platformio.ini
- Check that all library dependencies are installed
- Verify Unity test framework is available

### Tests Fail to Upload
- Check that ESP32 board is connected
- Verify correct COM port
- Ensure board has sufficient memory

### BLE Tests Fail
- Some tests require BLE to be available
- Ensure no other BLE devices are interfering
- Reset the board between test runs if needed

## References

- [Unity Test Framework](https://github.com/ThrowTheSwitch/Unity)
- [PlatformIO Unit Testing](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html)
- [BeamLink Documentation](../README.md)

