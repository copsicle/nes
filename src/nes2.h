#ifndef _NES2_H
#define _NES2_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define ID_STR "NES\x1A"
#define TRAINER_ADD 0x7000
#define TRAINER_SIZE 0x0200
#define PRG_UNITS 0x4000
#define CHR_UNITS 0x2000

#include "memory.h"

typedef struct 
{
    char ID[5];
    uint16_t PRGROM;
    uint16_t CHRROM;
    uint8_t FLAGS;
    uint16_t MAPPER;
    uint8_t PRGSHIFT;
    uint8_t CHRSHIFT;
    uint8_t TIMING;
    uint8_t SYSTYPE;
    uint8_t MISCROMS;
    uint8_t DEFEXP;
} nesheader;

uint8_t LOAD_ROM (FILE* rom, nesheader* head, memory* mem, cartridge* cart);

#endif
