#ifndef _MAPPER_H
#define _MAPPER_H

#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    uint16_t PRG_START;
    uint16_t PRG_END;
    uint16_t PRG_BANK_SIZE;
    uint8_t PRG_MAP_COUNT;
    uint16_t CHR_START;
    uint16_t CHR_END;
    uint16_t CHR_BANK_SIZE;
    uint8_t CHR_MAP_COUNT;
    uint16_t REG_START;
    uint16_t REG_END;
    uint16_t PRG_RAM_START;
    uint16_t PRG_RAM_END;
    uint8_t PRG_RAM_EN;
    uint16_t CHR_RAM_START;
    uint16_t CHR_RAM_END;
    uint8_t CHR_RAM_EN;
} mapper;
/*
typedef uint8_t (*mapper_ptr);
static mapper_ptr add;
*/

typedef struct 
{
    uint8_t** PRG;
    uint8_t** CHR;
    uint8_t** RAM;
    uint8_t* PRG_BANK;
    uint8_t* CHR_BANK;
} maps;

uint8_t ALLOC_MAPS (maps* mem, mapper* map);

static mapper suppprted_mappers[0x1000] = {
    [0] = {0x8000, 0xFFFF, 0x8000, 0, 0x0000, 0x0000, 0x0000, 0, 0x0000, 0x0000, 0x0000, 0x0000, 0, 0x0000, 0x0000, 0},
    [1] = {0x8000, 0xFFFF, 0x4000, 20, 0x0000, 0x1FFF, 0x1000, 32, 0x0000, 0x0000, 0x6000, 0x7FFF, 1, 0x0000, 0x0000, 0},
    [2] = {},
    [3] = {},
    [4] = {},
    [5] = {},
    [7] = {},
    [9] = {},
    [11] = {},
    [12] = {},
    [14] = {},
    [34] = {},
    [66] = {},
    [69] = {},
    [71] = {},
    };

#endif