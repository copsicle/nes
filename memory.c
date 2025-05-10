#include <stdint.h>
#include <stdlib.h>

#include "6502.h"
#include "memory.h"

uint8_t INIT_MEM(memory* mem)
{
    if (! (mem = (memory*) calloc(1, sizeof(memory))))
        return 1;
    if (! (mem->RAM = (uint8_t*) calloc(RAM_SIZE, sizeof(uint8_t))))
        return 1;
    if (! (mem->PPU = (uint8_t*) calloc(PPU_SIZE, sizeof(uint8_t))))
        return 1;
    if (! (mem->APUIO = (uint8_t*) calloc(APUIO_SIZE, sizeof(uint8_t))))
        return 1;
    if (! (mem->TEST = (uint8_t*) calloc(TEST_SIZE, sizeof(uint8_t))))
        return 1;
    return 0;
}

void FREE_MEM(memory* mem)
{
    free(mem->RAM);
    free(mem->PPU);
    free(mem->APUIO);
    free(mem->TEST);
    free(mem);
}

uint8_t* TRANSLATE_ADD (uint16_t add, memory* mem)
{
    if (add < 0x2000) return mem->RAM + (add % RAM_SIZE);
    else if (add < 0x4000) return mem->PPU + (add % PPU_SIZE);
    else if (add < 0x4018) return mem->APUIO + (add % APUIO_SIZE);
    else if (add < 0x4020) return mem->TEST + (add % TEST_SIZE);
    else return mem->CART + (add - (SPACE_SIZE - CART_SIZE));
}

uint8_t READ_MEM_BYTE (uint16_t add, uint8_t off, memory* mem, uint8_t* opr)
{
    uint16_t comb = add + off;
    opr = TRANSLATE_ADD(comb, mem);
    return 1 + ((add >> 8) != (comb >> 8)); 
}

uint8_t READ_MEM_WORD (uint16_t add, uint8_t off, memory* mem, uint16_t* opr)
{
    uint8_t* ptr = NULL;
    uint8_t cycle = READ_MEM_BYTE(add, off, mem, ptr);
    *opr = (uint16_t) *ptr;
    uint16_t comb = add + ((uint16_t)off) + 1;
    cycle += (off == 0xFF) + ((comb >> 8) != (add >> 8));
    cycle += READ_MEM_BYTE(comb, 0, mem, ptr);
    *opr |= (((uint16_t)*ptr) << 8);
    return cycle;
}

uint8_t IMM (registers* reg, memory* mem, uint8_t* opr)
{
    return READ_MEM_BYTE((reg->PC)++, 0, mem, opr);
}

uint8_t PTR (registers* reg, memory* mem, uint16_t* ptr)
{
    uint8_t cycle = READ_MEM_WORD(reg->PC, 0, mem, ptr);
    reg->PC += 2;
    return cycle;
}

uint8_t ZP (registers* reg, memory* mem, uint8_t* opr)
{
    uint8_t* add = NULL;
    uint8_t cycle = IMM(reg, mem, add);
    cycle += READ_MEM_BYTE(ZP_ADD, *add, mem, opr);
    return cycle;
}

uint8_t ZPX (registers* reg, memory* mem, uint8_t* opr)
{
    uint8_t* add = NULL;
    uint8_t cycle = IMM(reg, mem, add);
    uint8_t off = reg->X + *add;
    cycle += READ_MEM_BYTE(ZP_ADD, off, mem, opr);
    return 1 + cycle;
}

uint8_t ZPY (registers* reg, memory* mem, uint8_t* opr)
{
    uint8_t* add = NULL;
    uint8_t cycle = IMM(reg, mem, add);
    uint8_t off = reg->Y + *add;
    cycle += READ_MEM_BYTE(ZP_ADD, off, mem, opr);
    return 1 + cycle;
}

uint8_t ABS (registers* reg, memory* mem, uint8_t* opr)
{
    uint16_t abs = 0;
    uint8_t cycle = PTR(reg, mem, &abs);
    cycle += READ_MEM_BYTE(abs, 0, mem, opr);
    return cycle;
}

uint8_t ABX (registers* reg, memory* mem, uint8_t* opr)
{
    uint16_t abs = 0;
    uint8_t cycle = PTR(reg, mem, &abs);
    cycle += READ_MEM_BYTE(abs, reg->X, mem, opr);
    return cycle;
}

uint8_t ABY (registers* reg, memory* mem, uint8_t* opr)
{
    uint16_t abs = 0;
    uint8_t cycle = PTR(reg, mem, &abs);
    cycle += READ_MEM_BYTE(abs, reg->Y, mem, opr);
    return cycle;
}

uint8_t IN (registers* reg, memory* mem, uint8_t* opr)
{
    uint16_t ptr = 0;
    uint8_t cycle = PTR(reg, mem, &ptr);
    cycle += READ_MEM_WORD(ptr, 0, mem, &ptr);
    cycle += READ_MEM_BYTE(ptr, 0, mem, opr);
    return cycle;
}

uint8_t INDX (registers* reg, memory* mem, uint8_t* opr)
{
    uint8_t* add = NULL;
    uint8_t cycle = IMM(reg, mem, add);
    uint8_t off = *add + reg->X;
    uint16_t ptr = 0;
    cycle += READ_MEM_WORD(ZP_ADD, off, mem, &ptr);
    cycle += READ_MEM_BYTE(ptr, 0, mem, opr);
    return cycle;
}

uint8_t INDY (registers* reg, memory* mem, uint8_t* opr)
{
    uint8_t* add = 0;
    uint8_t cycle = IMM(reg, mem, add);
    uint16_t ptr = 0;
    cycle += READ_MEM_WORD(ZP_ADD, *add, mem, &ptr);
    cycle += READ_MEM_BYTE(ptr, reg->Y, mem, opr);
    return cycle;
}

uint8_t PUSH_BYTE (registers* reg, memory* mem, uint8_t opr)
{
    uint8_t* ptr = NULL;
    uint8_t cycle = READ_MEM_BYTE(STACK_ADD, (reg->S), mem, ptr);
    *ptr = opr;
    (reg->S)--;
    return cycle;
}

uint8_t POP_BYTE (registers* reg, memory* mem, uint8_t* opr)
{
    (reg->S)++;
    uint8_t cycle = READ_MEM_BYTE(STACK_ADD, (reg->S), mem, opr);
    return cycle;
}

uint8_t PUSH_WORD (registers* reg, memory* mem, uint16_t opr)
{
    uint8_t cycle = PUSH_BYTE(reg, mem, (uint8_t) opr);
    cycle += PUSH_BYTE(reg, mem, (uint8_t)(opr >> 8));
    return cycle;
}

uint8_t POP_WORD (registers* reg, memory* mem, uint16_t* opr)
{
    uint8_t* pop = NULL;
    uint8_t cycle = POP_BYTE(reg, mem, pop);
    *opr = ((uint16_t)*pop) << 8;
    cycle += POP_BYTE(reg, mem, pop);
    *opr |= (uint16_t)*pop;
    return cycle;
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
    uint8_t cycle = POP_BYTE(reg, mem, &flg);
    reg->P = (flg & 0xCF);
    return cycle;
}

uint8_t PIRQ (registers* reg, memory* mem, uint8_t* opr)
{
    (void) opr;
    uint8_t cycle = PUSH_WORD(reg, mem, ++(reg->PC));
    cycle += PSHP(reg, mem, NULL);
    return 1 + cycle;
}

uint8_t JMPB (registers* reg, memory* mem, uint8_t* opr)
{
    (void) opr;
    return READ_MEM_WORD(reg->PC, 0, mem, &reg->PC);
}

uint8_t JMPN (registers* reg, memory* mem, uint8_t* opr)
{
    (void) opr;
    uint8_t cycle = READ_MEM_WORD(reg->PC, 0, mem, &reg->PC);
    cycle += READ_MEM_WORD(reg->PC, 0, mem, &reg->PC);
    return cycle;
}

uint8_t JMPS (registers* reg, memory* mem, uint8_t* opr)
{
    (void) opr;
    uint16_t ptr = 0;
    uint8_t cycle = PTR(reg, mem, &ptr);
    cycle += PUSH_WORD(reg, mem, reg->PC);
    reg->PC = ptr;
    return 1 + cycle;
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
    uint8_t cycle = POP_WORD(reg,mem, &(reg->PC));
    (reg->PC)++;
    return 3 + cycle;
}
