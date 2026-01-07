# Embedded-Device-Driver
It is a bare-metal I2C device driver for the TMP102 temperature sensor on an STM32F4. It directly accesses I2C registers without HAL or Arduino, configures the sensor, reads raw data, performs two’s-complement conversion, and outputs temperature in °C using a simple main() loop.
