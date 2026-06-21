#ifndef HW_I2C_RP2040_I2C_H
#define HW_I2C_RP2040_I2C_H

#include "qemu/osdep.h"
#include "hw/sysbus.h"

#define TYPE_RP2040_I2C "rp2040-i2c"
#define RP2040_I2C(obj) OBJECT_CHECK(RP2040I2CState, (obj), TYPE_RP2040_I2C)

typedef struct RP2040I2CState {
    SysBusDevice parent_obj;
    MemoryRegion mmio;
    qemu_irq irq;
    uint32_t ctrl;
    uint32_t status;
    uint32_t data;
    uint32_t addr;
    uint32_t pointer;
    uint32_t phase;
    void *slave;
} RP2040I2CState;

#endif
