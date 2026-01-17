# ESP32 Telegram Bot Electricity Logger

This project implements an ESP32-based Telegram bot that monitors electricity availability by pinging a local IP address and logs status changes. It provides periodic updates to a Telegram channel and responds to admin commands.

## Features

- **Periodic Status Monitoring**: Automatically checks electricity status every configured interval and posts updates to a Telegram channel only when the status changes.
- **Admin Commands**:
  - `/start`: Shows available commands
  - `/status`: Displays comprehensive system information (IP, connection status, EEPROM write count)
- **Security**: Only responds to messages from the configured admin user ID; ignores all other messages.
- **Persistent Storage**: Uses EEPROM to store the last status and timestamp across reboots.
- **NTP Time Synchronization**: Uses GyverNTP for accurate timestamps.

## Hardware Requirements

- ESP32 or ESP8266 board
- Internet connection for Telegram API and NTP

## Software Setup

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- Telegram Bot Token (obtain from [@BotFather](https://t.me/botfather))
- Admin Telegram User ID

### Configuration

1. Clone the repository:
   ```bash
   git clone https://github.com/PetrenkoAnton/esp-tgbot-elogger.git
   cd esp-tgbot-elogger
   ```

2. Copy `platformio.ini.example` to `platformio.ini`:
   ```bash
   cp platformio.ini.example platformio.ini
   ```

3. Edit `platformio.ini` and update the build flags with your configuration:
   - `BOT_TOKEN`: Your Telegram bot token
   - `CHANNEL_ID`: Telegram channel ID for posting updates
   - `ADMIN_ID`: Your Telegram user ID (as admin)
   - `WIFI_SSID` and `WIFI_PASS`: WiFi credentials
   - `CHECK_IP`: IP address to ping for electricity status check
   - Other settings as needed

4. Build and upload:
   ```bash
   platformio run --target upload
   ```

### Usage

1. Power on the ESPxx device.
2. The bot will connect to WiFi, synchronize time via NTP, and start monitoring.
3. On startup, it posts an initial status message to the channel.
4. Every `INTERVAL` seconds, it checks the status and posts to the channel only if the electricity status has changed.
5. Send `/start` or `/status` to the bot (as admin) for manual information.

### Commands

- `/start`: Display help and available commands
- `/status`: Get current IP, electricity status, and EEPROM write count

All other messages are ignored for security.

## Project Structure

- `src/main.cpp`: Main application logic, setup, and loop
- `src/handler.cpp`: Telegram message handling
- `src/utils.cpp`: Utility functions for status checking and messaging
- `include/utils.h`: Header file with declarations
- `platformio.ini`: PlatformIO configuration

## Dependencies

- [FastBot2](https://github.com/GyverLibs/FastBot2): Telegram bot library
- [GyverNTP](https://github.com/GyverLibs/GyverNTP): NTP time synchronization
- ESP8266Ping or ESP32Ping: For network pinging

## License

This project is open-source. Please check the repository for license details.

## Contributing

Feel free to submit issues and pull requests.