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
    const mapper* MAP;
} cartridge;

uint8_t ALLOC_MAPS (cartridge* cart);
uint8_t* TRANSLATE_MAP (uint16_t add, cartridge* cart);

extern const mapper mapper_table[0x1000];

#endif
