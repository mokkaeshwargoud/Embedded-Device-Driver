/*
  TMP102 I2C Temperature Sensor 
  Platform: STM32F4 
  */

#include <stdint.h>

/* STM32 I2C1 Register Definitions */
#define I2C1_BASE      0x40005400UL
#define I2C1_CR1       (*(volatile uint32_t *)(I2C1_BASE + 0x00))
#define I2C1_CR2       (*(volatile uint32_t *)(I2C1_BASE + 0x04))
#define I2C1_DR        (*(volatile uint32_t *)(I2C1_BASE + 0x10))
#define I2C1_SR1       (*(volatile uint32_t *)(I2C1_BASE + 0x14))
#define I2C1_SR2       (*(volatile uint32_t *)(I2C1_BASE + 0x18))

/*  I2C Bit Definitions */
#define I2C_START      (1 << 8)
#define I2C_STOP       (1 << 9)
#define I2C_SB         (1 << 0)
#define I2C_ADDR       (1 << 1)
#define I2C_TXE        (1 << 7)
#define I2C_RXNE       (1 << 6)

/* TMP102 Definitions */
#define TMP102_ADDR        0x48
#define TMP102_TEMP_REG   0x00
#define TMP102_CFG_REG    0x01

/* Simple Delay */
static void delay(void)
{
    for (volatile uint32_t i = 0; i < 100000; i++);
}

/* Low-Level I2C Write */
static void i2c_write(uint8_t addr, uint8_t *data, uint8_t len)
{
    I2C1_CR1 |= I2C_START;
    while (!(I2C1_SR1 & I2C_SB));

    I2C1_DR = addr << 1;
    while (!(I2C1_SR1 & I2C_ADDR));
    (void)I2C1_SR2;

    for (uint8_t i = 0; i < len; i++)
    {
        I2C1_DR = data[i];
        while (!(I2C1_SR1 & I2C_TXE));
    }

    I2C1_CR1 |= I2C_STOP;
}

/* Low-Level I2C Read */
static void i2c_read(uint8_t addr, uint8_t *data, uint8_t len)
{
    I2C1_CR1 |= I2C_START;
    while (!(I2C1_SR1 & I2C_SB));

    I2C1_DR = (addr << 1) | 1;
    while (!(I2C1_SR1 & I2C_ADDR));
    (void)I2C1_SR2;

    for (uint8_t i = 0; i < len; i++)
    {
        while (!(I2C1_SR1 & I2C_RXNE));
        data[i] = I2C1_DR;
    }

    I2C1_CR1 |= I2C_STOP;
}

/* TMP102 Initialization */
void TMP102_Init(void)
{
    uint8_t cfg[3];

    cfg[0] = TMP102_CFG_REG;
    cfg[1] = 0x60;   // 12-bit resolution
    cfg[2] = 0xA0;   // Continuous conversion

    i2c_write(TMP102_ADDR, cfg, 3);
}

/* Read Temperature (°C)  */
float TMP102_ReadTemperature(void)
{
    uint8_t reg = TMP102_TEMP_REG;
    uint8_t rx[2];
    int16_t raw;

    i2c_write(TMP102_ADDR, &reg, 1);
    i2c_read(TMP102_ADDR, rx, 2);

    raw = (rx[0] << 4) | (rx[1] >> 4);

    if (raw & 0x800)        // Negative temperature
        raw |= 0xF000;

    return raw * 0.0625f;
}

/* APPLICATION ENTRY POINT */
int main(void)
{
    float temperature;

    /* Initialize TMP102 sensor */
    TMP102_Init();

    while (1)
    {
        temperature = TMP102_ReadTemperature();
        delay();   // Observe temperature via debugger
    }
}
