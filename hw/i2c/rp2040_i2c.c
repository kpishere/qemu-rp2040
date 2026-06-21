/*
 * Minimal RP2040 I2C controller stub for QEMU RP2040
 */

#include "qemu/osdep.h"
#include "hw/i2c/rp2040_i2c.h"
#include "hw/qdev-properties.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "hw/i2c/ads1015.h"

/* forward-declare optional ads1015 API */
extern uint16_t ads1015_master_write(void *opaque, uint32_t addr, uint16_t data);
extern uint16_t ads1015_master_read(void *opaque, uint32_t addr);

/* Register offsets */
#define I2C_CTRL    0x00
#define I2C_STATUS  0x04
#define I2C_DATA    0x08
#define I2C_ADDR    0x0C

static uint64_t rp2040_i2c_read(void *opaque, hwaddr offset, unsigned size)
{
    RP2040I2CState *s = opaque;
    uint32_t val = 0;

    switch (offset) {
    case I2C_CTRL:
        val = s->ctrl;
        break;
    case I2C_STATUS:
        val = s->status;
        break;
    case I2C_DATA:
        if (s->slave && s->phase >= 1) {
            s->data = ads1015_master_read(s->slave, s->pointer);
            s->status |= 0x1;
        }
        val = s->data;
        break;
    case I2C_ADDR:
        val = s->addr;
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "rp2040_i2c: bad read offset 0x%llx\n", (unsigned long long)offset);
    }

    return val;
}

static void rp2040_i2c_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)
{
    RP2040I2CState *s = opaque;

    switch (offset) {
    case I2C_CTRL:
        s->ctrl = value;
        break;
    case I2C_STATUS:
        s->status = value;
        break;
    case I2C_DATA:
        s->data = value & 0xFFFF;
        if (s->slave) {
            if (s->phase == 0) {
                s->pointer = (uint32_t)(s->data & 0xFF) & 0x03;
                s->phase = 1;
            } else {
                ads1015_master_write(s->slave, s->pointer, (uint16_t)s->data);
            }
        }
        s->status |= 0x1;
        break;
    case I2C_ADDR:
        s->addr = value & 0xFF;
        s->phase = 0;
        s->pointer = 0;
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "rp2040_i2c: bad write offset 0x%llx\n", (unsigned long long)offset);
    }
}

static const MemoryRegionOps rp2040_i2c_ops = {
    .read = rp2040_i2c_read,
    .write = rp2040_i2c_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
};

static void rp2040_i2c_reset(DeviceState *dev)
{
    RP2040I2CState *s = RP2040_I2C(dev);
    s->ctrl = 0;
    s->status = 0;
    s->data = 0;
    s->addr = 0;
    s->pointer = 0;
    s->phase = 0;
}

static void rp2040_i2c_init(Object *obj)
{
    RP2040I2CState *s = RP2040_I2C(obj);
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    s->pointer = 0;
    s->phase = 0;

    memory_region_init_io(&s->mmio, obj, &rp2040_i2c_ops, s, TYPE_RP2040_I2C, 0x1000);
    sysbus_init_mmio(sbd, &s->mmio);
    sysbus_init_irq(sbd, &s->irq);
}

static void rp2040_i2c_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    dc->reset = rp2040_i2c_reset;
}

static const TypeInfo rp2040_i2c_info = {
    .name = TYPE_RP2040_I2C,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RP2040I2CState),
    .instance_init = rp2040_i2c_init,
    .class_init = rp2040_i2c_class_init,
};

static void rp2040_i2c_register_types(void)
{
    type_register_static(&rp2040_i2c_info);
}

type_init(rp2040_i2c_register_types)
