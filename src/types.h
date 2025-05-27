#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>

typedef struct
{
    uint8_t A; // Accumulator
    uint8_t X; // X register
    uint8_t Y; // Y register
    uint16_t PC; // Program counter
    uint8_t S; // Stack Pointer
    uint8_t P; // Status Register
    
    uint16_t C; // Not a register, rather the clock freq
} registers;

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

typedef struct
{
    uint8_t **PTR;
    uint8_t *ARR;
    uint8_t COUNT;
} bank;

typedef struct 
{
    bank PRG;
    bank CHR;
    bank RAM;
    const mapper *MAP;
} cartridge;

typedef struct
{
    uint8_t *RAM;
    uint8_t *PPU;
    uint8_t *APUIO;
    uint8_t *TEST;
} memory;

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

typedef struct
{
    ppumem* MEM;
    
} ppu;

#endif
