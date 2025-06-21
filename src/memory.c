#include "types.h"
#include "mapper.h"
#include "memory.h"
/*
uint8_t INIT_MEM(nes con)
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

void FREE_MEM(nes con)
{
    free(mem->RAM);
    free(mem->PPU);
    free(mem->APUIO);
    free(mem->TEST);
}
*/
uint8_t *CPU_ADD (uint16_t add, nes con)
{
    switch (add / 0x2000) 
    {
        case 0:
            return con->CPU->RAM + (add % RAM_SIZE);
        case 1:
            return con->PPU->REG + (add % PPU_SIZE);
        case 2:
            if (add < 0x4014)
                return con->APU->REG + (add % APU_SIZE);
            else if (add < 0x4018)
                return con->CPU->IO + (add % IO_SIZE);
            else if (add < 0x4020)
                return NULL; // Unsupported
            __attribute__((fallthrough));
        default:
            return TRANSLATE_MAP(add, con);
    }
}

uint8_t READ_MEM_BYTE (uint16_t add, uint8_t off, nes con)
{
    uint16_t comb = add + off;
    con->CPU->OPR = CPU_ADD(comb, con);
    printf("Memory address 0x%04X was accessed\n", comb);
    if (add >= 0x2000 && add < 0x4000) 
        NULL; // TODO: HANDLE REGISTER READ
    return 1 + ((add >> 8) != (comb >> 8)); 
}

uint8_t READ_MEM_WORD (uint16_t add, uint8_t off, nes con, uint16_t *opr)
{
    uint8_t cycle = READ_MEM_BYTE(add, off, con);
    *opr = (uint16_t) *con->CPU->OPR;
    uint16_t comb = add + ((uint16_t)off) + 1;
    cycle += (off == 0xFF) + ((comb >> 8) != (add >> 8));
    cycle += READ_MEM_BYTE(comb, 0, con);
    *opr |= (((uint16_t)*con->CPU->OPR) << 8);
    return cycle;
}

uint8_t IMM (nes con)
{
    return READ_MEM_BYTE((con->CPU->PC)++, 0, con);
}

uint8_t PTR (nes con, uint16_t *ptr)
{
    uint8_t cycle = READ_MEM_WORD(con->CPU->PC, 0, con, ptr);
    con->CPU->PC += 2;
    return cycle;
}

uint8_t ZP (nes con)
{
    uint8_t cycle = IMM(con);
    cycle += READ_MEM_BYTE(ZP_ADD, *con->CPU->OPR, con);
    return cycle;
}

uint8_t ZPX (nes con)
{
    uint8_t cycle = IMM(con);
    uint8_t off = con->CPU->X + *con->CPU->OPR;
    cycle += READ_MEM_BYTE(ZP_ADD, off, con);
    return 1 + cycle;
}

uint8_t ZPY (nes con)
{
    uint8_t cycle = IMM(con);
    uint8_t off = con->CPU->Y + *con->CPU->OPR;
    cycle += READ_MEM_BYTE(ZP_ADD, off, con);
    return 1 + cycle;
}

uint8_t ABS (nes con)
{
    uint16_t abs = 0;
    uint8_t cycle = PTR(con, &abs);
    cycle += READ_MEM_BYTE(abs, 0, con);
    return cycle;
}

uint8_t ABX (nes con)
{
    uint16_t abs = 0;
    uint8_t cycle = PTR(con, &abs);
    cycle += READ_MEM_BYTE(abs, con->CPU->X, con);
    return cycle;
}

uint8_t ABY (nes con)
{
    uint16_t abs = 0;
    uint8_t cycle = PTR(con, &abs);
    cycle += READ_MEM_BYTE(abs, con->CPU->Y, con);
    return cycle;
}

uint8_t IN (nes con)
{
    uint16_t ptr = 0;
    uint8_t cycle = PTR(con, &ptr);
    cycle += READ_MEM_WORD(ptr, 0, con, &ptr);
    cycle += READ_MEM_BYTE(ptr, 0, con);
    return cycle;
}

uint8_t INDX (nes con)
{
    uint8_t cycle = IMM(con);
    uint8_t off = *con->CPU->OPR + con->CPU->X;
    uint16_t ptr = 0;
    cycle += READ_MEM_WORD(ZP_ADD, off, con, &ptr);
    cycle += READ_MEM_BYTE(ptr, 0, con);
    return cycle;
}

uint8_t INDY (nes con)
{
    uint8_t cycle = IMM(con);
    uint16_t ptr = 0;
    cycle += READ_MEM_WORD(ZP_ADD, *con->CPU->OPR, con, &ptr);
    cycle += READ_MEM_BYTE(ptr, con->CPU->Y, con);
    return cycle;
}

uint8_t PUSH_BYTE (nes con, uint8_t opr)
{
    uint8_t cycle = READ_MEM_BYTE(STACK_ADD, con->CPU->S, con);
    *con->CPU->OPR = opr;
    (con->CPU->S)--;
    return cycle;
}

uint8_t POP_BYTE (nes con)
{
    (con->CPU->S)++;
    uint8_t cycle = READ_MEM_BYTE(STACK_ADD, (con->CPU->S), con);
    return cycle;
}

uint8_t PUSH_WORD (nes con, uint16_t opr)
{
    uint8_t cycle = PUSH_BYTE(con, (uint8_t) opr);
    cycle += PUSH_BYTE(con, (uint8_t)(opr >> 8));
    return cycle;
}

uint8_t POP_WORD (nes con, uint16_t *opr)
{
    uint8_t cycle = POP_BYTE(con);
    *opr = ((uint16_t)*(con->CPU->OPR)) << 8;
    cycle += POP_BYTE(con);
    *opr |= (uint16_t)*(con->CPU->OPR);
    return cycle;
}

uint8_t PSHP (nes con)
{
    return PUSH_BYTE(con, con->CPU->P | 0x10);
}

uint8_t PLLP (nes con)
{
    uint8_t cycle = POP_BYTE(con);
    con->CPU->P = (*(con->CPU->OPR) & 0xCF);
    return cycle;
}

uint8_t PIRQ (nes con)
{
    uint8_t cycle = PUSH_WORD(con, ++(con->CPU->PC));
    cycle += PSHP(con);
    return 1 + cycle;
}

uint8_t JMPB (nes con)
{
    return READ_MEM_WORD(con->CPU->PC, 0, con, &con->CPU->PC);
}

uint8_t JMPN (nes con)
{
    uint8_t cycle = READ_MEM_WORD(con->CPU->PC, 0, con, &con->CPU->PC);
    cycle += READ_MEM_WORD(con->CPU->PC, 0, con, &con->CPU->PC);
    return cycle;
}

uint8_t JMPS (nes con)
{
    uint16_t ptr = 0;
    uint8_t cycle = PTR(con, &ptr);
    cycle += PUSH_WORD(con, con->CPU->PC);
    con->CPU->PC = ptr;
    return 1 + cycle;
}

uint8_t PSHA (nes con)
{
    return PUSH_BYTE(con, con->CPU->A);
}

uint8_t PLLA (nes con)
{
    uint8_t cycle = POP_BYTE(con);
    con->CPU->A = *(con->CPU->OPR);
    return cycle;
}

uint8_t PRTS (nes con)
{
    uint8_t cycle = POP_WORD(con, &(con->CPU->PC));
    (con->CPU->PC)++;
    return 3 + cycle;
}


const operation operation_table[0x100] = {
    /*     X0    X1    X2    X3    X4    X5    X6    X7    X8    X9    XA    XB    XC    XD    XE    XF*/
    /*0X*/ PIRQ, INDX, NULL, INDX, NULL, ZP  , ZP  , ZP  , PSHP, IMM , NULL, IMM , NULL, ABS , ABS , ABS,
    /*1X*/ IMM , INDY, NULL, INDY, NULL, ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , NULL, ABX , ABX , ABX,
    /*2X*/ JMPS, INDX, NULL, INDX, ZP  , ZP  , ZP  , ZP  , PLLP, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
    /*3X*/ IMM , INDY, NULL, INDY, NULL, ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , NULL, ABX , ABX , ABX,
    /*4X*/ NULL, INDX, NULL, INDX, NULL, ZP  , ZP  , ZP  , PSHA, IMM , NULL, IMM , JMPB, ABS , ABS , ABS,
    /*5X*/ IMM , INDY, NULL, INDY, NULL, ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , NULL, ABX , ABX , ABX,
    /*6X*/ PRTS, INDX, NULL, INDX, NULL, ZP  , ZP  , ZP  , PLLA, IMM , NULL, IMM , JMPN, ABS , ABS , ABS,
    /*7X*/ IMM , INDY, NULL, INDY, NULL, ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , NULL, ABX , ABX , ABX,
    /*8X*/ IMM , INDX, IMM , INDX, ZP  , ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
    /*9X*/ IMM , INDY, NULL, INDY, ZPX , ZPX , ZPY , ZPY , NULL, ABY , NULL, ABY , ABX , ABX , ABY , ABY,
    /*AX*/ IMM , INDX, IMM , INDX, ZP  , ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
    /*BX*/ IMM , INDY, NULL, INDY, ZPX , ZPX , ZPY , ZPY , NULL, ABY , NULL, ABY , ABX , ABX , ABY , ABY,
    /*CX*/ IMM , INDX, IMM , INDX, ZP  , ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
    /*DX*/ IMM , INDY, NULL, INDY, ZPX , ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , ABX , ABX , ABX , ABX,
    /*EX*/ IMM , INDX, IMM , INDX, ZP  , ZP  , ZP  , ZP  , NULL, IMM , NULL, IMM , ABS , ABS , ABS , ABS,
    /*FX*/ IMM , INDY, NULL, INDY, ZPX , ZPX , ZPX , ZPX , NULL, ABY , NULL, ABY , ABX , ABX , ABX , ABX
};
