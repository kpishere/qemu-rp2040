#ifndef HW_I2C_ADS1015_H
#define HW_I2C_ADS1015_H

#include "qemu/osdep.h"
#include "hw/sysbus.h"

#define TYPE_ADS1015 "ads1015"
#define RP2040_ADS(obj) OBJECT_CHECK(RP2040ADSState, (obj), TYPE_ADS1015)

#define ADS1015_REG_CONVERSION 0x00
#define ADS1015_REG_CONFIG     0x01
#define ADS1015_REG_LO_THRESH  0x02
#define ADS1015_REG_HI_THRESH  0x03

typedef struct RP2040ADSState {
    DeviceState parent_obj;
    uint16_t config;
    uint16_t lo_thresh;
    uint16_t hi_thresh;
    uint16_t sample;
} RP2040ADSState;

uint16_t ads1015_master_write(void *opaque, uint32_t reg, uint16_t data);
uint16_t ads1015_master_read(void *opaque, uint32_t reg);

#endif
