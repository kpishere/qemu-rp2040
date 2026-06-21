#ifndef HW_SSI_RP2040_SPI_H
#define HW_SSI_RP2040_SPI_H

#include "qemu/osdep.h"
#include "hw/sysbus.h"

#define TYPE_RP2040_SPI "rp2040-spi"
#define RP2040_SPI(obj) OBJECT_CHECK(RP2040SPIState, (obj), TYPE_RP2040_SPI)

typedef struct RP2040SPIState {
    SysBusDevice parent_obj;
    MemoryRegion mmio;
    qemu_irq irq;
    uint32_t ctrl;
    uint32_t status;
    uint32_t data;
    uint32_t clkdiv;
    void *slave;
} RP2040SPIState;

#endif
