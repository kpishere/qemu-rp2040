/*
 * RP2040 I2C/ADS1015 Thermistor Test Program
 * Reads a theoretical thermistor on ADS1015 over I2C0.
 */

#include <stdint.h>

/* UART registers */
#define UART0_BASE      0x40034000
#define UART0_DR        (UART0_BASE + 0x000)
#define UART0_FR        (UART0_BASE + 0x018)
#define UART0_CR        (UART0_BASE + 0x030)
#define UART_FR_TXFE    (1 << 7)
#define UART_CR_UARTEN  (1 << 0)
#define UART_CR_TXE     (1 << 8)
#define UART_CR_RXE     (1 << 9)

/* I2C0 registers */
#define I2C0_BASE       0x40044000
#define I2C0_CTRL       (I2C0_BASE + 0x00)
#define I2C0_STATUS     (I2C0_BASE + 0x04)
#define I2C0_DATA       (I2C0_BASE + 0x08)
#define I2C0_ADDR       (I2C0_BASE + 0x0C)

/* ADS1015 registers */
#define ADS1015_REG_CONVERSION 0x00
#define ADS1015_REG_CONFIG     0x01
#define ADS1015_REG_LO_THRESH  0x02
#define ADS1015_REG_HI_THRESH  0x03

static void uart_putc(char c) {
    while (!(*(volatile uint32_t*)UART0_FR & UART_FR_TXFE));
    *(volatile uint32_t*)UART0_DR = c;
}

static void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}

static void uart_puthex(uint32_t value) {
    const char hex[] = "0123456789ABCDEF";
    uart_puts("0x");
    for (int i = 28; i >= 0; i -= 4) {
        uart_putc(hex[(value >> i) & 0xF]);
    }
}

static void i2c0_set_address(uint8_t address) {
    *(volatile uint32_t*)I2C0_ADDR = address;
}

static void i2c0_write_data(uint16_t value) {
    *(volatile uint32_t*)I2C0_DATA = value;
}

static uint16_t i2c0_read_data(void) {
    return *(volatile uint32_t*)I2C0_DATA & 0xFFFF;
}

static void i2c0_set_control(uint32_t value) {
    *(volatile uint32_t*)I2C0_CTRL = value;
}

static uint32_t udiv(uint32_t numerator, uint32_t denominator) {
    uint32_t quotient = 0;
    uint32_t remainder = 0;

    for (int i = 31; i >= 0; i--) {
        remainder = (remainder << 1) | ((numerator >> i) & 1);
        if (remainder >= denominator) {
            remainder -= denominator;
            quotient |= 1u << i;
        }
    }

    return quotient;
}

int main(void) {
    *(volatile uint32_t*)UART0_CR = UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE;

    uart_puts("\nRP2040 I2C/ADS1015 Thermistor Test\n");
    uart_puts("===================================\n");

    uart_puts("Configuring I2C0 for ADS1015 access...\n");
    i2c0_set_address(0x48); /* ADS1015 default address */
    i2c0_set_control(0x1);  /* Enable I2C controller */

    uart_puts("Writing ADS1015 config register...\n");
    i2c0_write_data((ADS1015_REG_CONFIG << 8) | 0x8583);
    uart_puts("Reading ADS1015 conversion register...\n");
    i2c0_write_data((ADS1015_REG_CONVERSION << 8) | 0x0000);
    uint16_t raw = i2c0_read_data();

    uart_puts("  - ADS1015 conversion sample: ");
    uart_puthex(raw);
    uart_puts("\n");

    uart_puts("Interpreting as thermistor voltage: 0V @ 0°C, 3.3V @ 300°C\n");
    uint32_t temp_c = udiv(raw * 300, 0x7FF);
    uart_puts("  - Simulated temperature: ");
    uart_puthex(temp_c);
    uart_puts(" °C\n");

    uart_puts("Test complete for Ottawa weather (72°F / 22.2°C).\n");
    while (1);
    return 0;
}
