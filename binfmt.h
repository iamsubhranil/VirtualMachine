#ifndef BINFMT_H
#define BINFMT_H

#include<stdint.h>

#define MAGIC 0x564D4558 // VMEX
#define VERSION 7

/* Binary header and footer */

typedef struct {
    uint32_t magic;
    uint8_t version;
    uint16_t numIns;
} Header;

/* Binary Format */
#define FLEXIBLE 0x40
#define OPTIMISED 0x41

typedef struct {
    uint8_t format;
    uint8_t instructionLength;
} Footer;

#endif
