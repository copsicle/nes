#include <stdint.h>

#include "6502.h"
#include "memory.h"

uint8_t* INIT_MEM ()
{
    return NULL;
}

uint8_t READ_MEM_BYTE (uint16_t add, uint8_t off, memory* mem, uint8_t* opr)
{
    (void) add;
    (void) off;
    (void) mem;
    (void) opr;
    return 1; // ADD MEMORY MAPPING AND PAGE CHECKING WITH OFFSET
    // REMEMBER TO MAKE SURE THE ACTUAL POINTER FOR THE DATA IS PASSED
}

uint8_t READ_MEM_WORD (uint16_t add, uint8_t off, memory* mem, uint16_t* opr)
{
    (void) add;
    (void) off;
    (void) mem;
    (void) opr;
    return 2; // ADD MEMORY MAPPING AND PAGE CHECKING WITH OFFSET
    // REMEMBER TO MAKE SURE THE ACTUAL POINTER FOR THE DATA IS PASSED
}

uint8_t IMM (registers* reg, memory* mem, uint8_t* opr)
{
    return READ_MEM_BYTE((reg->PC)++, 0, mem, opr);
}

uint8_t PTR (registers* reg, memory* mem, uint16_t* ptr)
{
    uint8_t add = READ_MEM_WORD(reg->PC, 0, mem, ptr);
    reg->PC += 2;
    return add;
}

uint8_t ZP (registers* reg, memory* mem, uint8_t* opr)
{
    uint8_t add = 0;
    uint8_t imm = IMM(reg, mem, &add);
    *opr = mem->ZEROPAGE->ptr[add];
    return 1 + imm;
}

uint8_t ZPX (registers* reg, memory* mem, uint8_t* opr)
{
    uint8_t add = 0;
    uint8_t imm = IMM(reg, mem, &add);
    *opr = mem->ZEROPAGE->ptr[add + reg->X];
    return 2 + imm;
}

uint8_t ZPY (registers* reg, memory* mem, uint8_t* opr)
{
    uint8_t add = 0;
    uint8_t imm = IMM(reg, mem, &add);
    *opr = mem->ZEROPAGE->ptr[add + reg->Y];
    return 2 + imm;
}

uint8_t ABS (registers* reg, memory* mem, uint8_t* opr)
{
    uint16_t abs = 0;
    uint8_t ptr = PTR(reg, mem, &abs);
    return READ_MEM_BYTE(abs, 0, mem, opr) + ptr;
}

uint8_t ABX (registers* reg, memory* mem, uint8_t* opr)
{
    uint16_t abs = 0;
    uint8_t ptr = PTR(reg, mem, &abs);
    return READ_MEM_BYTE(abs, reg->X, mem, opr) + ptr;
}

uint8_t ABY (registers* reg, memory* mem, uint8_t* opr)
{
    uint16_t abs = 0;
    uint8_t ptr = PTR(reg, mem, &abs);
    return READ_MEM_BYTE(abs, reg->Y, mem, opr) + ptr;
}

uint8_t IN (registers* reg, memory* mem, uint8_t* opr)
{
    uint16_t ptr = 0;
    uint8_t ind1 = PTR(reg, mem, &ptr);
    uint8_t ind2 = READ_MEM_WORD(ptr, 0, mem, &ptr);
    return READ_MEM_BYTE(ptr, 0, mem, opr) + ind1 + ind2;
}

uint8_t INDX (registers* reg, memory* mem, uint8_t* opr)
{
    uint8_t add = 0;
    uint8_t imm = IMM(reg, mem, &add);
    uint16_t ptr = 0;
    uint8_t ind = READ_MEM_WORD(mem->ZEROPAGE->ptr[add], reg->X, mem, &ptr);
    return READ_MEM_BYTE(ptr, 0, mem, opr) + imm + ind;
}

uint8_t INDY (registers* reg, memory* mem, uint8_t* opr)
{
    uint8_t add = 0;
    uint8_t imm = IMM(reg, mem, &add);
    uint16_t ptr = 0;
    uint8_t ind = READ_MEM_WORD(mem->ZEROPAGE->ptr[add], 0, mem, &ptr);
    return READ_MEM_BYTE(ptr, reg->Y, mem, opr) + imm + ind;
}

uint8_t PUSH_BYTE (registers* reg, memory* mem, uint8_t opr)
{
    if (reg->S) mem->STACK->ptr[(reg->S)--] = opr;
    return 1;
}

uint8_t POP_BYTE (registers* reg, memory* mem, uint8_t* opr)
{
    if (reg->S < 0xFF)*opr = mem->STACK->ptr[++(reg->S)];
    return 1;
}

uint8_t PUSH_WORD (registers* reg, memory* mem, uint16_t opr)
{
    if (reg->S > 0x01)
    {
    mem->STACK->ptr[(reg->S)--] = (uint8_t)(opr >> 8);
    mem->STACK->ptr[(reg->S)--] = (uint8_t)opr;
    }
    return 2;
}

uint8_t POP_WORD (registers* reg, memory* mem, uint16_t* opr)
{
    if (reg->S < 0xFE)
        *opr = 
            ((uint16_t)mem->STACK->ptr[++(reg->S)]) |
            (((uint16_t)(mem->STACK->ptr[++(reg->S)])) << 8);
    return 2;
}

uint8_t PSHP (registers* reg, memory* mem, uint8_t* opr)
{
    (void) opr;
    return PUSH_BYTE(reg, mem, reg->P | 0x10);
}

uint8_t PLLP (registers* reg, memory* mem, uint8_t* opr)
{
    (void) opr;
    uint8_t flg = 0;
    uint8_t res = POP_BYTE(reg, mem, &flg);
    reg->P = (flg & 0xCF);
    return res;
}

uint8_t PIRQ (registers* reg, memory* mem, uint8_t* opr)
{
    (void) opr;
    return 1 +
        PUSH_WORD(reg, mem, ++(reg->PC)) +
        PSHP(reg, mem, NULL);
}

uint8_t JMPB (registers* reg, memory* mem, uint8_t* opr)
{
    (void) opr;
    return READ_MEM_WORD(reg->PC, 0, mem, &reg->PC);
}

uint8_t JMPN (registers* reg, memory* mem, uint8_t* opr)
{
    (void) opr;
    return 
        READ_MEM_WORD(reg->PC, 0, mem, &reg->PC) + 
        READ_MEM_WORD(reg->PC, 0, mem, &reg->PC);
}

uint8_t JMPS (registers* reg, memory* mem, uint8_t* opr)
{
    (void) opr;
    uint16_t ptr = 0;
    uint8_t ret = PTR(reg, mem, &ptr) +
        PUSH_WORD(reg, mem, reg->PC);
    reg->PC = ptr;
    return 1 + ret;
}

uint8_t PSHA (registers* reg, memory* mem, uint8_t* opr)
{
    return PUSH_BYTE(reg, mem, reg->A);
}

uint8_t PLLA (registers* reg, memory* mem, uint8_t* opr)
{
    return POP_BYTE(reg, mem, &reg->A);
}

uint8_t PRTS (registers* reg, memory* mem, uint8_t* opr)
{
    uint8_t pop = POP_WORD(reg,mem, &(reg->PC));
    (reg->PC)++;
    return 3 + pop;
}
