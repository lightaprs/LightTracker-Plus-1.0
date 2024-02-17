#include "logger.h"
#include <Arduino.h>
#include <WiFi.h>

#define WIFI_SSID     ""
#define WIFI_PASSWORD ""

logging::Logger logger;

void setup() {
  Serial.begin(115200);

  logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "MAIN", "This is a error");
  logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "MAIN", "This is a warning");
  logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "MAIN", "This is a info");
  logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "MAIN", "This is a debug");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "WIFI", ".");
  }
  logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "WIFI", "WiFi connected");
  logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "WIFI", "IP address: ");
  logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "WIFI", WiFi.localIP().toString().c_str());

  logger.setSyslogServer(IPAddress(10, 0, 0, 123), 514, "TestDevice");

  logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "MAIN", "This is a error");
  delay(100);
  logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "WIFI", "This is a warning");
  delay(100);
  logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "OTHER", "This is a info");
  delay(100);
  logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "SAME", "This is a debug");
}

uint8_t counter = 0;

void loop() {
  delay(5000);
  logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "SAME", "counter: %d", counter);
  counter++;
}
