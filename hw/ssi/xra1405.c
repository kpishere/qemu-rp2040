/* Minimal XRA1405 SPI GPIO expander emulation for QEMU RP2040 */

#include "qemu/osdep.h"
#include "hw/ssi/xra1405.h"
#include "qemu/log.h"

static void xra1405_reset(DeviceState *dev)
{
    RP2040XRAState *s = RP2040_XRA(dev);
    s->direction = 0;
    s->output = 0;
    s->input = 0xFF;
    s->polarity = 0;
}

uint16_t xra1405_receive(void *opaque, uint16_t data)
{
    RP2040XRAState *s = opaque;
    uint8_t reg = (data >> 8) & 0xFF;
    uint8_t value = data & 0xFF;
    uint16_t response = 0;

    switch (reg) {
    case XRA1405_REG_INPUT:
        response = (uint16_t)(((s->input & ~s->direction) | (s->output & s->direction)) ^ s->polarity);
        break;
    case XRA1405_REG_OUTPUT:
        s->output = value;
        response = s->output;
        break;
    case XRA1405_REG_DIRECTION:
        s->direction = value;
        response = s->direction;
        break;
    case XRA1405_REG_POLARITY:
        s->polarity = value;
        response = s->polarity;
        break;
    default:
        response = (uint16_t)value;
        break;
    }

    return response;
}

static void xra1405_init(Object *obj)
{
    RP2040XRAState *s = RP2040_XRA(obj);
    s->direction = 0;
    s->output = 0;
    s->input = 0xFF;
    s->polarity = 0;
}

static void xra1405_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    dc->reset = xra1405_reset;
}

static const TypeInfo xra1405_info = {
    .name = TYPE_XRA1405,
    .parent = TYPE_DEVICE,
    .instance_size = sizeof(RP2040XRAState),
    .instance_init = xra1405_init,
    .class_init = xra1405_class_init,
};

static void xra1405_register_types(void)
{
    type_register_static(&xra1405_info);
}

type_init(xra1405_register_types)
