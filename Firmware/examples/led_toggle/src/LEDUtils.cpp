#include "LEDUtils.h"

namespace LEDUtils {

void initLED(int pin, bool activeHigh, bool defaultState) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, activeHigh ? (defaultState ? HIGH : LOW) : (defaultState ? LOW : HIGH));
}

void turnOn(int pin, bool activeHigh) {
  digitalWrite(pin, activeHigh ? HIGH : LOW);
}

void turnOff(int pin, bool activeHigh) {
  digitalWrite(pin, activeHigh ? LOW : HIGH);
}

bool toggle(int pin, bool activeHigh) {
  bool currentState = isOn(pin, activeHigh);
  digitalWrite(pin, activeHigh ? (!currentState ? HIGH : LOW) : (!currentState ? LOW : HIGH));
  return !currentState;
}

bool isOn(int pin, bool activeHigh) {
  return digitalRead(pin) == (activeHigh ? HIGH : LOW);
}

void bootBlinkSequence(int pin, bool activeHigh, int blinkCount, 
                      unsigned long blinkDelay, bool finalState) {
  // Start ON
  turnOn(pin, activeHigh);
  delay(blinkDelay);

  // Blink sequence
  for (int i = 0; i < blinkCount; i++) {
    turnOff(pin, activeHigh);
    delay(blinkDelay);
    turnOn(pin, activeHigh);
    delay(blinkDelay);
  }

  // Set final state
  if (finalState) {
    turnOn(pin, activeHigh);
  } else {
    turnOff(pin, activeHigh);
  }
}

const char* getStatusString(int pin, bool activeHigh) {
  return isOn(pin, activeHigh) ? "ON" : "OFF";
}

} // namespace LEDUtils
