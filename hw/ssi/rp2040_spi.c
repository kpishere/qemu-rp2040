/* Minimal RP2040 SPI (SSI) controller stub for QEMU RP2040 */

#include "qemu/osdep.h"
#include "hw/ssi/rp2040_spi.h"
#include "hw/qdev-properties.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "hw/ssi/xra1405.h"

/* forward declare optional slave API */
extern uint16_t xra1405_receive(void *opaque, uint16_t data);

/* Register offsets */
#define SPI_CTRL    0x00
#define SPI_STATUS  0x04
#define SPI_DATA    0x08
#define SPI_CLKDIV  0x0C

static uint64_t rp2040_spi_read(void *opaque, hwaddr offset, unsigned size)
{
    RP2040SPIState *s = opaque;
    uint32_t val = 0;

    switch (offset) {
    case SPI_CTRL: val = s->ctrl; break;
    case SPI_STATUS: val = s->status; break;
    case SPI_DATA: val = s->data; break;
    case SPI_CLKDIV: val = s->clkdiv; break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "rp2040_spi: bad read offset 0x%llx\n", (unsigned long long)offset);
    }

    return val;
}

static void rp2040_spi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)
{
    RP2040SPIState *s = opaque;

    switch (offset) {
    case SPI_CTRL:
        s->ctrl = value;
        break;
    case SPI_STATUS:
        s->status = value;
        break;
    case SPI_DATA:
        s->data = value & 0xFFFF;
        /* forward to attached slave (if any) and capture response */
        if (s->slave) {
            uint16_t resp = xra1405_receive(s->slave, (uint16_t)s->data);
            s->data = resp;
        }
        s->status |= 0x1;
        break;
    case SPI_CLKDIV:
        s->clkdiv = value;
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "rp2040_spi: bad write offset 0x%llx\n", (unsigned long long)offset);
    }
}

static const MemoryRegionOps rp2040_spi_ops = {
    .read = rp2040_spi_read,
    .write = rp2040_spi_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
};

static void rp2040_spi_reset(DeviceState *dev)
{
    RP2040SPIState *s = RP2040_SPI(dev);
    s->ctrl = 0;
    s->status = 0;
    s->data = 0;
    s->clkdiv = 0;
}

static void rp2040_spi_init(Object *obj)
{
    RP2040SPIState *s = RP2040_SPI(obj);
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);

    memory_region_init_io(&s->mmio, obj, &rp2040_spi_ops, s, TYPE_RP2040_SPI, 0x1000);
    sysbus_init_mmio(sbd, &s->mmio);
    sysbus_init_irq(sbd, &s->irq);
}

static void rp2040_spi_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    dc->reset = rp2040_spi_reset;
}

static const TypeInfo rp2040_spi_info = {
    .name = TYPE_RP2040_SPI,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RP2040SPIState),
    .instance_init = rp2040_spi_init,
    .class_init = rp2040_spi_class_init,
};

static void rp2040_spi_register_types(void)
{
    type_register_static(&rp2040_spi_info);
}

type_init(rp2040_spi_register_types)
