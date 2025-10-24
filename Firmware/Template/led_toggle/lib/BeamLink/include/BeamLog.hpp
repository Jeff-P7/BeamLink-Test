#pragma once
#include <Arduino.h>

// ---- Build switches (optional; can also be set via platformio.ini) ----
// #define BEAMLOG_DISABLE_COLOR  // force no ANSI colors
// #define BEAMLOG_DISABLE_EMOJI  // strip emojis
// #define BEAMLOG_DISABLE_DEBUG  // remove LOG_DBG (saves flash)

// ANSI codes
#if !defined(BEAMLOG_DISABLE_COLOR)
  #define BLK_CLR_RESET  "\x1b[0m"
  #define BLK_CLR_DIM    "\x1b[2m"
  #define BLK_FG_RED     "\x1b[31m"
  #define BLK_FG_GRN     "\x1b[32m"
  #define BLK_FG_YEL     "\x1b[33m"
  #define BLK_FG_BLU     "\x1b[34m"
  #define BLK_FG_CYN     "\x1b[36m"
  #define BLK_FG_MAG     "\x1b[35m"
#else
  #define BLK_CLR_RESET  ""
  #define BLK_CLR_DIM    ""
  #define BLK_FG_RED     ""
  #define BLK_FG_GRN     ""
  #define BLK_FG_YEL     ""
  #define BLK_FG_BLU     ""
  #define BLK_FG_CYN     ""
  #define BLK_FG_MAG     ""
#endif

// Emojis
#if !defined(BEAMLOG_DISABLE_EMOJI)
  #define BLK_EMJ_OK   u8"✅ "
  #define BLK_EMJ_INFO u8"ℹ️  "
  #define BLK_EMJ_WARN u8"⚠️  "
  #define BLK_EMJ_ERR  u8"❌ "
  #define BLK_EMJ_BLE  u8"📶 "
  #define BLK_EMJ_CFG  u8"🗂️  "
  #define BLK_EMJ_PIN  u8"🔌 "
#else
  #define BLK_EMJ_OK   ""
  #define BLK_EMJ_INFO ""
  #define BLK_EMJ_WARN ""
  #define BLK_EMJ_ERR  ""
  #define BLK_EMJ_BLE  ""
  #define BLK_EMJ_CFG  ""
  #define BLK_EMJ_PIN  ""
#endif

// Safe printf wrapper
inline void bl_print(const char* fmt, ...) {
  char buffer[256];
  va_list args; 
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  Serial.print(buffer);
}

// Timestamp (ms since boot)
inline void bl_stamp() {
  bl_print(BLK_CLR_DIM "[%8lu ms]" BLK_CLR_RESET " ", millis());
}

// Enhanced error logging with file and function info
#define LOG_ERR_LOC(fmt, ...) \
  do { \
    bl_stamp(); \
    bl_print(BLK_FG_RED "%s" fmt " [%s:%s]" BLK_CLR_RESET "\n", \
             BLK_EMJ_ERR, ##__VA_ARGS__, __FILE__, __FUNCTION__); \
  } while(0)

// Core macro
#define BL_LOG_RAW(color, emoji, fmt, ...) \
  do { bl_stamp(); bl_print(color "%s" fmt BLK_CLR_RESET "\n", emoji, ##__VA_ARGS__); } while(0)

// Public APIs
#define LOG_OK(fmt, ...)    BL_LOG_RAW(BLK_FG_GRN, BLK_EMJ_OK,   fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  BL_LOG_RAW(BLK_FG_CYN, BLK_EMJ_INFO, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  BL_LOG_RAW(BLK_FG_YEL, BLK_EMJ_WARN, fmt, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...)   BL_LOG_RAW(BLK_FG_RED, BLK_EMJ_ERR,  fmt, ##__VA_ARGS__)

#if !defined(BEAMLOG_DISABLE_DEBUG)
  #define LOG_DBG(fmt, ...) BL_LOG_RAW(BLK_CLR_DIM, "", fmt, ##__VA_ARGS__)
#else
  #define LOG_DBG(fmt, ...) do{}while(0)
#endif

// Key=Value helper
#define LOG_KV(key, valueFmt, ...) \
  do { bl_stamp(); bl_print(BLK_CLR_DIM "%s=" BLK_CLR_RESET valueFmt "\n", key, ##__VA_ARGS__); } while(0)

// Optional domain helpers
#define LOG_BLE(fmt, ...)  BL_LOG_RAW(BLK_FG_MAG, BLK_EMJ_BLE, fmt, ##__VA_ARGS__)
#define LOG_CFG(fmt, ...)  BL_LOG_RAW(BLK_FG_BLU, BLK_EMJ_CFG, fmt, ##__VA_ARGS__)
#define LOG_PIN(fmt, ...)  BL_LOG_RAW(BLK_FG_GRN, BLK_EMJ_PIN, fmt, ##__VA_ARGS__)
