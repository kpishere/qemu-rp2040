#ifndef HW_SSI_XRA1405_H
#define HW_SSI_XRA1405_H

#include "qemu/osdep.h"
#include "hw/sysbus.h"

#define TYPE_XRA1405 "xra1405"
#define RP2040_XRA(obj) OBJECT_CHECK(RP2040XRAState, (obj), TYPE_XRA1405)

#define XRA1405_REG_INPUT      0x00
#define XRA1405_REG_OUTPUT     0x01
#define XRA1405_REG_DIRECTION  0x02
#define XRA1405_REG_POLARITY   0x03

typedef struct RP2040XRAState {
    DeviceState parent_obj;
    /* XRA1405 internal register state */
    uint8_t dir;
    uint8_t out;
    uint8_t in;
    uint8_t polarity;
} RP2040XRAState;

/* Master-forwarded receive function: returns 16-bit response */
uint16_t xra1405_receive(void *opaque, uint16_t data);

#endif
