#ifndef _PPU_H
#define _PPU_H

#include <stdint.h>

#define PATTERN_PAGE 0x1000
#define NAMETABLE_PAGE 0x0400
#define UNUSED_PAGE 0x0F00
#define PALLETE_SIZE 0x0020
#define PPU_SPACE 0x4000
#define OAM_SPACE 0x0100

typedef struct
{
    uint8_t *PT0;
    uint8_t *PT1;
    uint8_t *NT0;
    uint8_t *NT1;
    uint8_t *NT2;
    uint8_t *NT3;
    uint8_t *PLT;
    uint8_t *OAM;
} ppumem;

#endif
