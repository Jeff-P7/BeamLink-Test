#pragma once
#include <Arduino.h>

// ANSI color helpers for serial logs
#define C_RESET  "\x1b[0m"
#define C_DIM    "\x1b[2m"
#define C_RED    "\x1b[31m"
#define C_GREEN  "\x1b[32m"
#define C_YELLOW "\x1b[33m"
#define C_CYAN   "\x1b[36m"
#define C_MAGENTA "\x1b[35m"
#define C_BLUE   "\x1b[34m"

// Emoji and styled logging functions
inline void log_ok(const char* msg) { Serial.printf(C_GREEN "✅ %s" C_RESET "\n", msg); }
inline void log_info(const char* msg) { Serial.printf(C_CYAN "ℹ️  %s" C_RESET "\n", msg); }
inline void log_warn(const char* msg) { Serial.printf(C_YELLOW "⚠️  %s" C_RESET "\n", msg); }
inline void log_err(const char* msg) { Serial.printf(C_RED "❌ %s" C_RESET "\n", msg); }
inline void log_success(const char* msg) { Serial.printf(C_GREEN "🎉 %s" C_RESET "\n", msg); }
inline void log_config(const char* msg) { Serial.printf(C_MAGENTA "⚙️  %s" C_RESET "\n", msg); }
inline void log_ble(const char* msg) { Serial.printf(C_BLUE "📡 %s" C_RESET "\n", msg); }
inline void log_led(const char* msg) { Serial.printf(C_YELLOW "💡 %s" C_RESET "\n", msg); }
inline void log_sensor(const char* msg) { Serial.printf(C_CYAN "🌡️  %s" C_RESET "\n", msg); }
inline void log_heartbeat(const char* msg) { Serial.printf(C_DIM "💓 %s" C_RESET "\n", msg); }
