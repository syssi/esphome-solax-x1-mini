# Solax Meter Gateway Guide for Zero Export Control

## Introduction

The Solax Meter Gateway is an ESPHome-based project that enables zero export control for Solax inverters via RS485. This system dynamically adjusts the inverter's power output based on real-time power consumption data from a smart meter, ensuring that no excess power is fed back into the grid.

## Background

The primary function of this setup is to achieve "zero export" or "zero feed-in" to the grid. It works by:

1. Continuously monitoring the current power consumption from a smart meter.
2. Adjusting the Solax inverter's power output to match the household consumption.
3. Preventing any excess solar power from being exported to the grid.

This approach optimizes self-consumption of solar energy and complies with regulations that restrict or prohibit feeding power back into the grid.

## Key Features

- Acts as a drop-in replacement for SDM230-MID-SOLAX (SDM230 with Solax firmware).
- Compatible with Solax inverters that have the "export control: meter enabled" setting.
- Responds to the inverter's periodic requests for current power consumption.
- Retrieves sensor measurements from another sensor/device to provide consumption data.
- Potentially supports a wider range of Solax inverters beyond the X1 series.

## Requirements

- A Solax inverter with export control capability
- An ESP8266 or ESP32 board (e.g., Wemos D1 Mini)
- A RS485 to TTL converter module
- A smart meter or power monitoring device to provide real-time consumption data
- Basic soldering skills and tools
- A computer with ESPHome installed

## Hardware Setup

1. Connect the RS485 to TTL converter to your ESP board:
   - VCC to 3.3V
   - GND to GND
   - RX to TX pin (default GPIO4)
   - TX to RX pin (default GPIO5)

2. Connect the RS485 side to your Solax inverter:
   - A+ to pin 4 of the RJ45 connector
   - B- to pin 5 of the RJ45 connector
   - GND to pin 7 of the RJ45 connector

3. Ensure you have access to the power consumption data from your smart meter or power monitoring device.

## Software Setup

1. Install ESPHome if you haven't already: `pip install esphome`

2. Create a new ESPHome project or use the provided YAML configuration.

3. Modify the YAML configuration:
- Set your Wi-Fi credentials
- Adjust MQTT settings if you're using MQTT
- Modify pins if necessary
- Configure the data source for power consumption (e.g., MQTT topic of the smart meter)

4. Flash the ESP board with the ESPHome configuration.

## Inverter Configuration

To enable the "export control: meter" mode on your Solax inverter:

1. Navigate to Settings
2. Enter PIN: 6868
3. Go to "Export control"
4. Select "Meter enable"

## Usage

Once set up, the Solax Meter Gateway provides several features:

1. **Power Monitoring**: It reads the instantaneous power consumption from your smart meter or power monitoring device.

2. **Dynamic Power Adjustment**: Based on the current consumption, it adjusts the inverter's power output to achieve zero export.

3. **Operation Mode**: You can see the current operation mode (Auto, Manual, Off).

4. **Manual Control**: You can switch to manual mode and set a specific power demand.

5. **Emergency Power Off**: A safety feature to quickly shut off the inverter.

6. **Automatic Inactivity Detection**: It will trigger a fault if no data is received for a set period.

Access these features through your home automation system (e.g., Home Assistant) or MQTT, depending on your setup.

## Troubleshooting

- Ensure all connections are correct and secure.
- Check the ESPHome logs for any error messages.
- Verify that your inverter model is supported and correctly configured for export control.
- If you encounter issues, increase the log level in the YAML configuration for more detailed information.
- Verify that the power consumption data is being received correctly from your monitoring device.

## Conclusion

The Solax Meter Gateway provides a flexible and powerful interface for zero export control with Solax inverters. It can be used with various Solax models that support the export control feature, potentially extending beyond the X1 series. By accurately matching your solar power production to your current consumption, you can optimize self-consumption and ensure compliance with grid feed-in regulations.
