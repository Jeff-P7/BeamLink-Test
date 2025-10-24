#include "BeamUtils.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace BeamUtils {

bool parseCommand(const std::string& message, std::string& command, std::string& action) {
  size_t pos = message.find(':');
  if (pos == std::string::npos || pos == 0 || pos == message.length() - 1) {
    return false;
  }
  
  command = message.substr(0, pos);
  action = message.substr(pos + 1);
  return true;
}

bool parseCommandValue(const std::string& message, std::string& command, 
                       std::string& action, std::string& value) {
  size_t firstColon = message.find(':');
  if (firstColon == std::string::npos) {
    return false;
  }
  
  size_t secondColon = message.find(':', firstColon + 1);
  if (secondColon == std::string::npos) {
    return false;
  }
  
  command = message.substr(0, firstColon);
  action = message.substr(firstColon + 1, secondColon - firstColon - 1);
  value = message.substr(secondColon + 1);
  
  return !command.empty() && !action.empty();
}

std::map<std::string, std::string> parseKeyValue(const std::string& message) {
  std::map<std::string, std::string> result;
  
  std::vector<std::string> pairs = split(message, ',');
  for (const auto& pair : pairs) {
    size_t eqPos = pair.find('=');
    if (eqPos != std::string::npos && eqPos > 0 && eqPos < pair.length() - 1) {
      std::string key = trim(pair.substr(0, eqPos));
      std::string value = trim(pair.substr(eqPos + 1));
      result[key] = value;
    }
  }
  
  return result;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;
  
  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }
  
  return tokens;
}

std::string trim(const std::string& str) {
  size_t first = str.find_first_not_of(" \t\n\r");
  if (first == std::string::npos) return "";
  
  size_t last = str.find_last_not_of(" \t\n\r");
  return str.substr(first, (last - first + 1));
}

std::string toLower(const std::string& str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(), ::tolower);
  return result;
}

std::string toUpper(const std::string& str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(), ::toupper);
  return result;
}

bool startsWith(const std::string& str, const std::string& prefix) {
  if (prefix.length() > str.length()) return false;
  return str.compare(0, prefix.length(), prefix) == 0;
}

bool endsWith(const std::string& str, const std::string& suffix) {
  if (suffix.length() > str.length()) return false;
  return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

std::string formatStats(uint32_t received, uint32_t sent, uint32_t errors, unsigned long uptimeMs) {
  std::string result = "Stats: ";
  result += "RX=" + std::to_string(received);
  result += ", TX=" + std::to_string(sent);
  result += ", Errors=" + std::to_string(errors);
  result += ", Uptime=" + formatUptime(uptimeMs);
  return result;
}

std::string formatUptime(unsigned long uptimeMs) {
  unsigned long seconds = uptimeMs / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  
  seconds %= 60;
  minutes %= 60;
  hours %= 24;
  
  std::string result;
  if (days > 0) result += std::to_string(days) + "d ";
  if (hours > 0 || days > 0) result += std::to_string(hours) + "h ";
  if (minutes > 0 || hours > 0 || days > 0) result += std::to_string(minutes) + "m ";
  result += std::to_string(seconds) + "s";
  
  return result;
}

} // namespace BeamUtils

