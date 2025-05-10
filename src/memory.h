#ifndef _MEMORY_H
#define _MEMORY_H

#include "6502.h"
#include <stdint.h>

#define RAM_SIZE 0x0800
#define PPU_SIZE 0x0008
#define APUIO_SIZE 0x0018
#define TEST_SIZE 0x0008
#define CART_SIZE 0xBFE0
#define SPACE_SIZE 0x10000

#define ZP_ADD 0x0000
#define STACK_ADD 0x0100

typedef struct
{
    uint8_t* RAM;
    uint8_t* PPU;
    uint8_t* APUIO;
    uint8_t* TEST;
    uint8_t* CART;
} memory;

uint8_t INIT_MEM(memory* mem);
void FREE_MEM(memory* mem);

uint8_t IMM (registers* reg, memory* mem, uint8_t* opr);
uint8_t PTR (registers* reg, memory* mem, uint16_t* ptr);
uint8_t ZP (registers* reg, memory* mem, uint8_t* opr);
uint8_t ZPX (registers* reg, memory* mem, uint8_t* opr);
uint8_t ZPY (registers* reg, memory* mem, uint8_t* opr);
uint8_t ABS (registers* reg, memory* mem, uint8_t* opr);
uint8_t ABX (registers* reg, memory* mem, uint8_t* opr);
uint8_t ABY (registers* reg, memory* mem, uint8_t* opr);
uint8_t IN (registers* reg, memory* mem, uint8_t* opr);
uint8_t INDX (registers* reg, memory* mem, uint8_t* opr);
uint8_t INDY (registers* reg, memory* mem, uint8_t* opr);

uint8_t PUSH_BYTE (registers* reg, memory* mem, uint8_t opr);
uint8_t POP_BYTE (registers* reg, memory* mem, uint8_t* opr);
uint8_t PUSH_WORD(registers* reg, memory* mem, uint16_t opr);
uint8_t POP_WORD(registers* reg, memory* mem, uint16_t* opr);

uint8_t PIRQ (registers* reg, memory* mem, uint8_t* opr);
uint8_t PRTS (registers* reg, memory* mem, uint8_t* opr);

uint8_t JMPB (registers* reg, memory* mem, uint8_t* opr);
uint8_t JMPN (registers* reg, memory* mem, uint8_t* opr);
uint8_t JMPS (registers* reg, memory* mem, uint8_t* opr);

uint8_t PSHP (registers* reg, memory* mem, uint8_t* opr);
uint8_t PLLP (registers* reg, memory* mem, uint8_t* opr);
uint8_t PSHA (registers* reg, memory* mem, uint8_t* opr);
uint8_t PLLA (registers* reg, memory* mem, uint8_t* opr);

typedef uint8_t (*address_function_ptr) (registers*, memory*, uint8_t*);

static address_function_ptr addressing_modes[0x100] = {
//     X0    X1    X2    X3    X4    X5    X6    X7    X8    X9    XA    XB    XC    XD    XE    XF
/*0X*/ PIRQ, INDX, NULL, INDX, NULL, ZP  , ZP  , ZP  , PSHP, IMM , NULL, IMM , NULL, ABS , ABS , ABS,
/*1X*/ NULL, INDY, NULL, INDY, NULL, ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , NULL, ABX , ABX , ABX,
/*2X*/ JMPS, INDX, NULL, INDX, ZP  , ZP  , ZP  , ZP  , PLLP, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
/*3X*/ NULL, INDY, NULL, INDY, NULL, ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , NULL, ABX , ABX , ABX,
/*4X*/ NULL, INDX, NULL, INDX, NULL, ZP  , ZP  , ZP  , PSHA, IMM , NULL, IMM , JMPB, ABS , ABS , ABS,
/*5X*/ NULL, INDY, NULL, INDY, NULL, ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , NULL, ABX , ABX , ABX,
/*6X*/ PRTS, INDX, NULL, INDX, NULL, ZP  , ZP  , ZP  , PLLA, IMM , NULL, IMM , JMPN, ABS , ABS , ABS,
/*7X*/ NULL, INDY, NULL, INDY, NULL, ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , NULL, ABX , ABX , ABX,
/*8X*/ IMM , INDX, IMM , INDX, ZP  , ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
/*9X*/ NULL, INDY, NULL, INDY, ZPX , ZPX , ZPY , ZPY , NULL, ABY , NULL, ABY , ABX , ABX , ABY , ABY,
/*AX*/ IMM , INDX, IMM , INDX, ZP  , ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
/*BX*/ NULL, INDY, NULL, INDY, ZPX , ZPX , ZPY , ZPY , NULL, ABY , NULL, ABY , ABX , ABX , ABY , ABY,
/*CX*/ IMM , INDX, IMM , INDX, ZP  , ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
/*DX*/ NULL, INDY, NULL, INDY, ZPX , ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , ABX , ABX , ABX , ABX,
/*EX*/ IMM , INDX, IMM , INDX, ZP  , ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
/*FX*/ NULL, INDY, NULL, INDY, ZPX , ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , ABX , ABX , ABX , ABX
};

#endif