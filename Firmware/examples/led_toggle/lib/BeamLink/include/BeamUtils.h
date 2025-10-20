#pragma once
#include <string>
#include <map>
#include <vector>

/**
 * @file BeamUtils.h
 * @brief Utility functions for BeamLink message parsing
 * 
 * This file provides helper functions for parsing structured messages,
 * including JSON-like key-value pairs and command:action format.
 */

namespace BeamUtils {

  /**
   * @brief Parse a command:action string
   * 
   * Splits a string of format "command:action" into two parts.
   * 
   * @param message The message to parse
   * @param command Output parameter for the command part
   * @param action Output parameter for the action part
   * @return true if successfully parsed, false if format is invalid
   * 
   * @example
   * ```cpp
   * std::string cmd, act;
   * if (parseCommand("led:on", cmd, act)) {
   *   // cmd = "led", act = "on"
   * }
   * ```
   */
  bool parseCommand(const std::string& message, std::string& command, std::string& action);

  /**
   * @brief Parse a command:action:value string
   * 
   * Splits a string of format "command:action:value" into three parts.
   * 
   * @param message The message to parse
   * @param command Output parameter for the command part
   * @param action Output parameter for the action part
   * @param value Output parameter for the value part
   * @return true if successfully parsed, false if format is invalid
   */
  bool parseCommandValue(const std::string& message, std::string& command, 
                         std::string& action, std::string& value);

  /**
   * @brief Parse simple JSON-like key-value pairs
   * 
   * Parses a string like "key1=val1,key2=val2" into a map.
   * 
   * @param message The message to parse
   * @return Map of key-value pairs
   * 
   * @example
   * ```cpp
   * auto params = parseKeyValue("pin=2,state=on");
   * // params["pin"] = "2", params["state"] = "on"
   * ```
   */
  std::map<std::string, std::string> parseKeyValue(const std::string& message);

  /**
   * @brief Split a string by delimiter
   * 
   * @param str String to split
   * @param delimiter Character to split on
   * @return Vector of string parts
   */
  std::vector<std::string> split(const std::string& str, char delimiter);

  /**
   * @brief Trim whitespace from both ends of string
   * 
   * @param str String to trim
   * @return Trimmed string
   */
  std::string trim(const std::string& str);

  /**
   * @brief Convert string to lowercase
   * 
   * @param str String to convert
   * @return Lowercase string
   */
  std::string toLower(const std::string& str);

  /**
   * @brief Convert string to uppercase
   * 
   * @param str String to convert
   * @return Uppercase string
   */
  std::string toUpper(const std::string& str);

  /**
   * @brief Check if string starts with prefix
   * 
   * @param str String to check
   * @param prefix Prefix to look for
   * @return true if str starts with prefix
   */
  bool startsWith(const std::string& str, const std::string& prefix);

  /**
   * @brief Check if string ends with suffix
   * 
   * @param str String to check
   * @param suffix Suffix to look for
   * @return true if str ends with suffix
   */
  bool endsWith(const std::string& str, const std::string& suffix);

  /**
   * @brief Create a formatted statistics string
   * 
   * @param received Number of messages received
   * @param sent Number of messages sent
   * @param errors Number of errors
   * @param uptimeMs Uptime in milliseconds
   * @return Formatted string with statistics
   */
  std::string formatStats(uint32_t received, uint32_t sent, uint32_t errors, unsigned long uptimeMs);

  /**
   * @brief Format uptime into human-readable string
   * 
   * @param uptimeMs Uptime in milliseconds
   * @return Formatted string (e.g., "1d 2h 3m 4s")
   */
  std::string formatUptime(unsigned long uptimeMs);

} // namespace BeamUtils

