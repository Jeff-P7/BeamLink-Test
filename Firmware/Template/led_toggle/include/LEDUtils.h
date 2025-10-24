#pragma once

#include <Arduino.h>

/**
 * @brief LED control utilities for BeamLink LED examples
 * 
 * Provides common LED control functions for LED control examples.
 * This is specific to LED examples and not part of the core BeamLink library.
 */

namespace LEDUtils {

/**
 * @brief Initialize LED pin and set default state
 * @param pin GPIO pin number for LED
 * @param activeHigh true if LED is active high, false if active low
 * @param defaultState true for ON, false for OFF
 */
void initLED(int pin, bool activeHigh = true, bool defaultState = true);

/**
 * @brief Turn LED ON
 * @param pin GPIO pin number for LED
 * @param activeHigh true if LED is active high, false if active low
 */
void turnOn(int pin, bool activeHigh = true);

/**
 * @brief Turn LED OFF
 * @param pin GPIO pin number for LED
 * @param activeHigh true if LED is active high, false if active low
 */
void turnOff(int pin, bool activeHigh = true);

/**
 * @brief Toggle LED state
 * @param pin GPIO pin number for LED
 * @param activeHigh true if LED is active high, false if active low
 * @return true if LED is now ON, false if OFF
 */
bool toggle(int pin, bool activeHigh = true);

/**
 * @brief Check if LED is currently ON
 * @param pin GPIO pin number for LED
 * @param activeHigh true if LED is active high, false if active low
 * @return true if LED is ON, false if OFF
 */
bool isOn(int pin, bool activeHigh = true);

/**
 * @brief Perform boot blink sequence
 * @param pin GPIO pin number for LED
 * @param activeHigh true if LED is active high, false if active low
 * @param blinkCount number of blinks (default: 2)
 * @param blinkDelay delay between blinks in ms (default: 150)
 * @param finalState final LED state after sequence (default: true = ON)
 */
void bootBlinkSequence(int pin, bool activeHigh = true, int blinkCount = 2, 
                      unsigned long blinkDelay = 150, bool finalState = true);

/**
 * @brief Get LED status as string
 * @param pin GPIO pin number for LED
 * @param activeHigh true if LED is active high, false if active low
 * @return "ON" or "OFF"
 */
const char* getStatusString(int pin, bool activeHigh = true);

} // namespace LEDUtils
