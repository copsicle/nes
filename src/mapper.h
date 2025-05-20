#ifndef _MAPPER_H
#define _MAPPER_H

#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    uint16_t START;
    uint16_t END;
    uint16_t SIZE;
    uint8_t COUNT;
} memtype;

typedef struct
{
    memtype PRG;
    memtype CHR;
    memtype RAM;
    memtype PRG_NV;
    memtype CHR_RAM;
    memtype CHR_NV;
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
    [0] = {{0x8000, 0xFFFF, 0x8000, 1}, {0x0000, 0x1FFF, 0x2000, 1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
    [1] = {{0x8000, 0xFFFF, 0x4000, 20}, {0x0000, 0x1FFF, 0x1000, 32}, {0x6000, 0x7FFF, 0x2000, 1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
    /*
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
    */
    };

#endif
