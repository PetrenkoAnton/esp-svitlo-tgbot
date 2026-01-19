# ESP32 Telegram Bot Event Logger Project Instructions

## IMPORTANT
Don't auto commit your changes.
Always write short commit messages.
Use shortcut "CP" for "Commit and push current changes".

This is an ESP32-based project that implements a Telegram bot for event logging. It uses the PlatformIO build system with the Arduino framework.

## Project Structure
- `src/main.cpp`: Main application code
- `include/`: Header files
- `lib/`: Custom libraries
- `test/`: Unit tests

## Coding Guidelines
- Use C++ with Arduino framework
- Main functions: `setup()` for initialization, `loop()` for main logic
- Include `Arduino.h` for Arduino-specific functions
- Follow ESP32 and Arduino best practices
- Use Serial for debugging output
- Implement WiFi connectivity for Telegram bot functionality
- Handle event logging with appropriate data structures

## Dependencies
- Arduino framework for ESP32
- Telegram bot library (likely UniversalTelegramBot or similar)
- WiFi and HTTP client libraries

## Build and Run
- Use PlatformIO for building and uploading
- Ensure ESP32 board is selected in platformio.ini
- Monitor serial output for debugging

## AI Assistant Notes
- Prefer Arduino-style coding over raw ESP-IDF
- Suggest using FreeRTOS tasks for concurrent operations
- Recommend proper error handling and watchdog resets
- Encourage modular code with separate functions for bot commands, logging, etc.