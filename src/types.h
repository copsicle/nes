#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>

typedef enum
{
    FETCH = 0x00,
    MEMOP = 0x01,
    WORK = 0x02
} cpuphase;

typedef struct
{
    uint8_t A; // Accumulator
    uint8_t X; // X register
    uint8_t Y; // Y register
    uint16_t PC; // Program counter
    uint8_t S; // Stack Pointer
    uint8_t P; // Status Register
    
    uint8_t CLK; // Not a register, rather the clock freq
    uint8_t RAM[0x0800];
    uint8_t IO[0x04];
    uint8_t *OPR;

    cpuphase PHS;
} processor, *cpu;

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
    uint8_t NTCOUNT;
} mapper, *map;

typedef struct
{
    uint8_t **PTR;
    uint8_t *ARR;
    uint8_t COUNT;
} membank, *bank;

typedef struct 
{
    membank PRG;
    membank CHR;
    membank RAM;
    membank NT;
    mapper *MAP;
} cartridge, *cart;
/*
typedef struct
{
    uint8_t *RAM;
    uint8_t *PPU;
    uint8_t *APUIO;
    uint8_t *TEST;
} memory;
*/
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
} nesheader, *head;
/*
typedef struct
{
    uint8_t *PT0;
    uint8_t *PT1;
    uint8_t *NT0;
    uint8_t *NT1;
    uint8_t *NT2;
    uint8_t *NT3;
} ppumem;

typedef enum
{
    PPUCTRL = 0x00,
    PPUMASK = 0x01,
    PPUSTAT = 0x02,
    OAMADDR = 0x03,
    OAMDATA = 0x04,
    PPUSCRL = 0x05,
    PPUADDR = 0x06,
    PPUDATA = 0x07,
} ppuregs;

typedef enum
{
    FETCH_INST = 0x00,
    FETCH_MEM = 0x01,
    WORK = 0x02
} cpuphase, *cphase;
*/
typedef struct
{
    //ppumem MEM;
    //uint8_t *REG;
    uint16_t V;
    uint16_t T;
    uint16_t CYC;
    uint16_t SCL;
    uint8_t X;
    uint8_t W;
    uint8_t BUF;
    uint8_t SPR;
    uint8_t HIT;
    uint8_t EVN;

    uint8_t REG[0x08];

    uint16_t BGL;
    uint16_t BGH;
    uint8_t SPL[0x08];
    uint8_t SPH[0x08];
    uint8_t ATL;
    uint8_t ATH;

    uint8_t SPX[0x08];
    uint8_t SPA[0x08];

    uint8_t CLK;

    uint8_t PLT[0x20];
    uint8_t OAM[0x0100];
    uint8_t OAS[0x20];
} graphics, *ppu;

typedef struct
{
    uint8_t REG[0x14];
} audio, *apu;

typedef struct
{
    head HEAD;
    cpu CPU;
    ppu PPU;
    cart CART;
    apu APU;
} console, *nes;

typedef uint8_t (*operation) (nes);

#endif
