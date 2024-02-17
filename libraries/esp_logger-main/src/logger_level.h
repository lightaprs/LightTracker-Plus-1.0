#ifndef LOGGER_LEVEL_H_
#define LOGGER_LEVEL_H_

#include <Arduino.h>

namespace logging {

#undef LOG_RESET_COLOR
#undef LOG_COLOR_E
#undef LOG_COLOR_W
#undef LOG_COLOR_I
#undef LOG_COLOR_D
#undef LOG_COLOR_V

#define LOG_COLOR_BLACK  "30"
#define LOG_COLOR_RED    "31"
#define LOG_COLOR_GREEN  "32"
#define LOG_COLOR_BROWN  "33"
#define LOG_COLOR_BLUE   "34"
#define LOG_COLOR_PURPLE "35"
#define LOG_COLOR_CYAN   "36"
#define LOG_COLOR(COLOR) "\033[0;" COLOR "m"
#define LOG_BOLD(COLOR)  "\033[1;" COLOR "m"
#define LOG_RESET_COLOR  "\033[0m"
#define LOG_COLOR_E      LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W      LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I      LOG_COLOR(LOG_COLOR_GREEN)
#define LOG_COLOR_D      LOG_COLOR(LOG_COLOR_BLUE)

class LoggerLevel {
public:
  enum Value : uint8_t
  {
    LOGGER_LEVEL_ERROR = 3, // Critical errors
    LOGGER_LEVEL_WARN  = 4, // Error conditions but not critical
    LOGGER_LEVEL_INFO  = 6, // Information messages
    LOGGER_LEVEL_DEBUG = 7, // Extra information - default level (if not changed)
  };

  LoggerLevel() = default;

  constexpr LoggerLevel(Value aType) : _value(aType) {
  }
  constexpr bool operator==(LoggerLevel a) const {
    return _value == a._value;
  }
  constexpr bool operator!=(LoggerLevel a) const {
    return _value != a._value;
  }
  constexpr bool operator<=(LoggerLevel a) const {
    return _value <= a._value;
  }

  String toString() const {
    switch (_value) {
    case LOGGER_LEVEL_ERROR:
      return "ERROR";
    case LOGGER_LEVEL_WARN:
      return "WARN";
    case LOGGER_LEVEL_INFO:
      return "INFO";
    case LOGGER_LEVEL_DEBUG:
      return "DEBUG";
    default:
      return "";
    }
  }

  String getLineColor() const {
    switch (_value) {
    case LOGGER_LEVEL_ERROR:
      return LOG_COLOR_E;
    case LOGGER_LEVEL_WARN:
      return LOG_COLOR_W;
    case LOGGER_LEVEL_INFO:
      return LOG_COLOR_I;
    case LOGGER_LEVEL_DEBUG:
      return LOG_COLOR_D;
    default:
      return "";
    }
  }

  String resetColor() const {
    return LOG_RESET_COLOR;
  }

  Value getValue() const {
    return _value;
  }

private:
  Value _value;
};
} // namespace logging

#endif
