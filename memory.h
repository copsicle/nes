#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdint.h>

struct memory
{
    uint8_t* ZEROPAGE;
    uint8_t* STACK;
    uint8_t* GENERAL;
    uint8_t* PPU;
    uint8_t* APUIO;
    uint8_t* CARTRAM;
    uint8_t* CARTROM;
};

uint8_t IMM (struct registers* reg, struct memory* mem, uint8_t* opr);
uint8_t PTR (struct registers* reg, struct memory* mem, uint16_t* ptr);
uint8_t ZP (struct registers* reg, struct memory* mem, uint8_t* opr);
uint8_t ZPX (struct registers* reg, struct memory* mem, uint8_t* opr);
uint8_t ZPY (struct registers* reg, struct memory* mem, uint8_t* opr);
uint8_t ABS (struct registers* reg, struct memory* mem, uint8_t* opr);
uint8_t ABX (struct registers* reg, struct memory* mem, uint8_t* opr);
uint8_t ABY (struct registers* reg, struct memory* mem, uint8_t* opr);
uint8_t IN (struct registers* reg, struct memory* mem, uint8_t* opr);
uint8_t INDX (struct registers* reg, struct memory* mem, uint8_t* opr);
uint8_t INDY (struct registers* reg, struct memory* mem, uint8_t* opr);

uint8_t PUSH_BYTE (struct registers* reg, struct memory* mem, uint8_t opr);
uint8_t POP_BYTE (struct registers* reg, struct memory* mem, uint8_t* opr);
uint8_t PUSH_WORD(struct registers* reg, struct memory* mem, uint16_t opr);
uint8_t POP_WORD(struct registers* reg, struct memory* mem, uint16_t* opr);

uint8_t PIRQ (struct registers* reg, struct memory* mem, uint8_t* opr);

uint8_t JMPB (struct registers* reg, struct memory* mem, uint8_t* opr);
uint8_t JMPN (struct registers* reg, struct memory* mem, uint8_t* opr);

typedef uint8_t (*address_function_ptr) (struct registers*, struct memory*, uint8_t*);

static address_function_ptr addressing_modes[0x100] = {
//     X0    X1    X2    X3    X4    X5    X6    X7    X8    X9    XA    XB    XC    XD    XE    XF
/*00*/ PIRQ, INDX, NULL, INDX, NULL, ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , NULL, ABS , ABS , ABS,
/*10*/ NULL, INDY, NULL, INDY, NULL, ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , NULL, ABX , ABX , ABX,
/*20*/ ABS , INDX, NULL, INDX, ZP  , ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
/*30*/ NULL, INDY, NULL, INDY, NULL, ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , NULL, ABX , ABX , ABX,
/*40*/ NULL, INDX, NULL, INDX, NULL, ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , JMPB, ABS , ABS , ABS,
/*50*/ NULL, INDY, NULL, INDY, NULL, ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , NULL, ABX , ABX , ABX,
/*60*/ NULL, INDX, NULL, INDX, NULL, ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , JMPN, ABS , ABS , ABS,
/*70*/ NULL, INDY, NULL, INDY, NULL, ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , NULL, ABX , ABX , ABX,
/*80*/ IMM , INDX, IMM , INDX, ZP  , ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
/*90*/ NULL, INDY, NULL, INDY, ZPX , ZPX , ZPY , ZPY , NULL, ABY , NULL, ABY , ABX , ABX , ABY , ABY,
/*A0*/ IMM , INDX, IMM , INDX, ZP  , ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
/*B0*/ NULL, INDY, NULL, INDY, ZPX , ZPX , ZPY , ZPY , NULL, ABY , NULL, ABY , ABX , ABX , ABY , ABY,
/*C0*/ IMM , INDX, IMM , INDX, ZP  , ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
/*D0*/ NULL, INDY, NULL, INDY, ZPX , ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , ABX , ABX , ABX , ABX,
/*E0*/ IMM , INDX, IMM , INDX, ZP  , ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
/*F0*/ NULL, INDY, NULL, INDY, ZPX , ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , ABX , ABX , ABX , ABX
};

#endif