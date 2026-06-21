/*
 * RP2040 SPI/XRA1405 Test Program
 * Exercise SPI0 and the XRA1405 GPIO expander on pin 0.
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

/* SPI0 registers */
#define SPI0_BASE       0x4003C000
#define SPI0_CTRL       (SPI0_BASE + 0x00)
#define SPI0_STATUS     (SPI0_BASE + 0x04)
#define SPI0_DATA       (SPI0_BASE + 0x08)
#define SPI0_CLKDIV     (SPI0_BASE + 0x0C)

/* XRA1405 register commands */
#define XRA1405_REG_INPUT      0x00
#define XRA1405_REG_OUTPUT     0x01
#define XRA1405_REG_DIRECTION  0x02
#define XRA1405_REG_POLARITY   0x03

static void uart_putc(char c) {
    while (!(*(volatile uint32_t*)UART0_FR & UART_FR_TXFE));
    *(volatile uint32_t*)UART0_DR = c;
}

static void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') {
            uart_putc('\r');
        }
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

static uint32_t spi0_transfer(uint8_t reg, uint8_t value) {
    uint32_t command = ((uint32_t)reg << 8) | value;
    *(volatile uint32_t*)SPI0_DATA = command;
    return *(volatile uint32_t*)SPI0_DATA;
}

int main(void) {
    /* Enable UART for console output */
    *(volatile uint32_t*)UART0_CR = UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE;

    uart_puts("\nRP2040 SPI/XRA1405 Test Program\n");
    uart_puts("================================\n");

    uart_puts("Configuring XRA1405 pin 0 as output...\n");
    uint32_t response = spi0_transfer(XRA1405_REG_DIRECTION, 0x01);
    uart_puts("  - Direction response: ");
    uart_puthex(response);
    uart_puts("\n\n");

    uart_puts("Toggling theoretical LED on XRA1405 pin 0...\n");
    for (int i = 0; i < 4; i++) {
        uint8_t value = (i % 2) ? 0x01 : 0x00;
        response = spi0_transfer(XRA1405_REG_OUTPUT, value);
        uart_puts("  - LED ");
        uart_puts(value ? "ON" : "OFF");
        uart_puts(" (output register = ");
        uart_puthex(response);
        uart_puts(")\n");
    }

    uart_puts("\nSPI/XRA1405 test complete!\n");
    while (1);
    return 0;
}
