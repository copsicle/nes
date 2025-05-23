#include "memory.h"

uint8_t INIT_MEM(memory* mem)
{
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
}

uint8_t* TRANSLATE_ADD (uint16_t add, memory* mem, cartridge* cart)
{
    if (add < 0x2000) return mem->RAM + (add % RAM_SIZE);
    else if (add < 0x4000) return mem->PPU + (add % PPU_SIZE);
    else if (add < 0x4018) return mem->APUIO + (add % APUIO_SIZE);
    else if (add < 0x4020) return mem->TEST + (add % TEST_SIZE);
    return TRANSLATE_MAP(add, cart);
    //else return mem->CART + (add - (SPACE_SIZE - CART_SIZE));
}

uint8_t READ_MEM_BYTE (uint16_t add, uint8_t off, memory* mem, cartridge* cart, uint8_t** opr)
{
    uint16_t comb = add + off;
    *opr = TRANSLATE_ADD(comb, mem, cart);
    return 1 + ((add >> 8) != (comb >> 8)); 
}

uint8_t READ_MEM_WORD (uint16_t add, uint8_t off, memory* mem, cartridge* cart, uint16_t* opr)
{
    uint8_t* ptr = NULL;
    uint8_t cycle = READ_MEM_BYTE(add, off, mem, cart, &ptr);
    *opr = (uint16_t) *ptr;
    uint16_t comb = add + ((uint16_t)off) + 1;
    cycle += (off == 0xFF) + ((comb >> 8) != (add >> 8));
    cycle += READ_MEM_BYTE(comb, 0, mem, cart, &ptr);
    *opr |= (((uint16_t)*ptr) << 8);
    return cycle;
}

uint8_t IMM (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    return READ_MEM_BYTE((reg->PC)++, 0, mem, cart, opr);
}

uint8_t PTR (registers* reg, memory* mem, cartridge* cart, uint16_t* ptr)
{
    uint8_t cycle = READ_MEM_WORD(reg->PC, 0, mem, cart, ptr);
    reg->PC += 2;
    return cycle;
}

uint8_t ZP (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    uint8_t* add = NULL;
    uint8_t cycle = IMM(reg, mem, cart, &add);
    cycle += READ_MEM_BYTE(ZP_ADD, *add, mem, cart, opr);
    return cycle;
}

uint8_t ZPX (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    uint8_t* add = NULL;
    uint8_t cycle = IMM(reg, mem, cart, &add);
    uint8_t off = reg->X + *add;
    cycle += READ_MEM_BYTE(ZP_ADD, off, mem, cart, opr);
    return 1 + cycle;
}

uint8_t ZPY (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    uint8_t* add = NULL;
    uint8_t cycle = IMM(reg, mem, cart, &add);
    uint8_t off = reg->Y + *add;
    cycle += READ_MEM_BYTE(ZP_ADD, off, mem, cart, opr);
    return 1 + cycle;
}

uint8_t ABS (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    uint16_t abs = 0;
    uint8_t cycle = PTR(reg, mem, cart, &abs);
    cycle += READ_MEM_BYTE(abs, 0, mem, cart, opr);
    return cycle;
}

uint8_t ABX (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    uint16_t abs = 0;
    uint8_t cycle = PTR(reg, mem, cart, &abs);
    cycle += READ_MEM_BYTE(abs, reg->X, mem, cart, opr);
    return cycle;
}

uint8_t ABY (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    uint16_t abs = 0;
    uint8_t cycle = PTR(reg, mem, cart, &abs);
    cycle += READ_MEM_BYTE(abs, reg->Y, mem, cart, opr);
    return cycle;
}

uint8_t IN (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    uint16_t ptr = 0;
    uint8_t cycle = PTR(reg, mem, cart, &ptr);
    cycle += READ_MEM_WORD(ptr, 0, mem, cart, &ptr);
    cycle += READ_MEM_BYTE(ptr, 0, mem, cart, opr);
    return cycle;
}

uint8_t INDX (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    uint8_t* add = NULL;
    uint8_t cycle = IMM(reg, mem, cart, &add);
    uint8_t off = *add + reg->X;
    uint16_t ptr = 0;
    cycle += READ_MEM_WORD(ZP_ADD, off, mem, cart, &ptr);
    cycle += READ_MEM_BYTE(ptr, 0, mem, cart, opr);
    return cycle;
}

uint8_t INDY (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    uint8_t* add = 0;
    uint8_t cycle = IMM(reg, mem, cart, &add);
    uint16_t ptr = 0;
    cycle += READ_MEM_WORD(ZP_ADD, *add, mem, cart, &ptr);
    cycle += READ_MEM_BYTE(ptr, reg->Y, mem, cart, opr);
    return cycle;
}

uint8_t PUSH_BYTE (registers* reg, memory* mem, cartridge* cart, uint8_t opr)
{
    uint8_t* ptr = NULL;
    uint8_t cycle = READ_MEM_BYTE(STACK_ADD, (reg->S), mem, cart, &ptr);
    *ptr = opr;
    (reg->S)--;
    return cycle;
}

uint8_t POP_BYTE (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    (reg->S)++;
    uint8_t cycle = READ_MEM_BYTE(STACK_ADD, (reg->S), mem, cart, opr);
    return cycle;
}

uint8_t PUSH_WORD (registers* reg, memory* mem, cartridge* cart, uint16_t opr)
{
    uint8_t cycle = PUSH_BYTE(reg, mem, cart, (uint8_t) opr);
    cycle += PUSH_BYTE(reg, mem, cart, (uint8_t)(opr >> 8));
    return cycle;
}

uint8_t POP_WORD (registers* reg, memory* mem, cartridge* cart, uint16_t* opr)
{
    uint8_t* pop = NULL;
    uint8_t cycle = POP_BYTE(reg, mem, cart, &pop);
    *opr = ((uint16_t)*pop) << 8;
    cycle += POP_BYTE(reg, mem, cart, &pop);
    *opr |= (uint16_t)*pop;
    return cycle;
}

uint8_t PSHP (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    (void) opr;
    return PUSH_BYTE(reg, mem, cart, reg->P | 0x10);
}

uint8_t PLLP (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    (void) opr;
    uint8_t* flg = NULL;
    uint8_t cycle = POP_BYTE(reg, mem, cart, &flg);
    reg->P = ((*flg) & 0xCF);
    return cycle;
}

uint8_t PIRQ (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    (void) opr;
    uint8_t cycle = PUSH_WORD(reg, mem, cart, ++(reg->PC));
    cycle += PSHP(reg, mem, cart, NULL);
    return 1 + cycle;
}

uint8_t JMPB (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    (void) opr;
    return READ_MEM_WORD(reg->PC, 0, mem, cart, &reg->PC);
}

uint8_t JMPN (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    (void) opr;
    uint8_t cycle = READ_MEM_WORD(reg->PC, 0, mem, cart, &reg->PC);
    cycle += READ_MEM_WORD(reg->PC, 0, mem, cart, &reg->PC);
    return cycle;
}

uint8_t JMPS (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    (void) opr;
    uint16_t ptr = 0;
    uint8_t cycle = PTR(reg, mem, cart, &ptr);
    cycle += PUSH_WORD(reg, mem, cart, reg->PC);
    reg->PC = ptr;
    return 1 + cycle;
}

uint8_t PSHA (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    (void) opr;
    return PUSH_BYTE(reg, mem, cart, reg->A);
}

uint8_t PLLA (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    (void) opr;
    uint8_t* add = &(reg->A);
    return POP_BYTE(reg, mem, cart, &add);
}

uint8_t PRTS (registers* reg, memory* mem, cartridge* cart, uint8_t** opr)
{
    (void) opr;
    uint8_t cycle = POP_WORD(reg, mem, cart, &(reg->PC));
    (reg->PC)++;
    return 3 + cycle;
}


const operation operation_table[0x100] = {
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
