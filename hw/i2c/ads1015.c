/* Minimal ADS1015 I2C ADC emulation for QEMU RP2040 */

#include "qemu/osdep.h"
#include "hw/i2c/ads1015.h"
#include "qemu/log.h"

static void ads1015_reset(DeviceState *dev)
{
    RP2040ADSState *s = RP2040_ADS(dev);
    s->config = 0x8583; /* default config: single-shot, AIN0, 4.096V */
    s->lo_thresh = 0x0000;
    s->hi_thresh = 0x8000;
    s->sample = 0x7FF;
}

uint16_t ads1015_master_write(void *opaque, uint32_t reg, uint16_t data)
{
    RP2040ADSState *s = opaque;
    switch (reg) {
    case ADS1015_REG_CONFIG:
        s->config = data;
        if (data & (1 << 8)) {
            s->sample = 0x7FF;
        }
        break;
    case ADS1015_REG_LO_THRESH:
        s->lo_thresh = data;
        break;
    case ADS1015_REG_HI_THRESH:
        s->hi_thresh = data;
        break;
    default:
        break;
    }
    return 0;
}

uint16_t ads1015_master_read(void *opaque, uint32_t reg)
{
    RP2040ADSState *s = opaque;
    switch (reg) {
    case ADS1015_REG_CONVERSION:
        return (uint16_t)(s->sample << 4);
    case ADS1015_REG_CONFIG:
        return s->config;
    case ADS1015_REG_LO_THRESH:
        return s->lo_thresh;
    case ADS1015_REG_HI_THRESH:
        return s->hi_thresh;
    default:
        return 0;
    }
}

static void ads1015_init(Object *obj)
{
    RP2040ADSState *s = RP2040_ADS(obj);
    s->config = 0x8583;
    s->lo_thresh = 0x0000;
    s->hi_thresh = 0x8000;
    s->sample = 0x7FF;
}

static void ads1015_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    dc->reset = ads1015_reset;
}

static const TypeInfo ads1015_info = {
    .name = TYPE_ADS1015,
    .parent = TYPE_DEVICE,
    .instance_size = sizeof(RP2040ADSState),
    .instance_init = ads1015_init,
    .class_init = ads1015_class_init,
};

static void ads1015_register_types(void)
{
    type_register_static(&ads1015_info);
}

type_init(ads1015_register_types)
