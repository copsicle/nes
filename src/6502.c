#include "types.h"
#include "6502.h"

/*
bit - flag
0 - (C)arry
1 - (Z)ero
2 - (I)nterrupt Disable
3 - (D)ecimal (Since BCD is disabled this is an extra flag)

4 - (B)reak (Disabled in the CPU, does not hold state)
5 - (1) Always 1
6 - (O)verflow
7 - (N)egative
*/

void RESET_CPU (registers *reg)
{
    reg->PC = 0xFFFC;
    reg->S -= 3;
    reg->P |= 0x24;
}

void PRINT_CPU (registers *reg)
{
    printf("CPU Dump:\n");
    printf("PC: 0x%04X | ", reg->PC);
    printf("SP: 0x%02X (%d)\n", reg->S, reg->S);
    printf("A: 0x%02X (%d) | ", reg->A, reg->A);
    printf("X: 0x%02X (%d) | ", reg->X, reg->X);
    printf("Y: 0x%02X (%d)\n", reg->A, reg->A);
    printf("FLAGS: 0x%02X\n\n", reg->P);
}

void SET_FLAGS (uint8_t *P, uint8_t mask, uint16_t *wres, uint8_t *opr1, uint8_t *opr2)
{
    // This helper function sets commonly used flags utilizing either 16 or 8 bit integers
    // The mask is the bits the particular function wishes to set
    *P &= ~mask;
    if (mask & 0x01) 
    {
        if (wres) *P |= (uint8_t)(*wres >> 8);
        else *P |= (((int8_t)*opr1) >= 0);
    }
    if (mask & 0x02) 
    {
        if (wres) *P |= (((uint8_t)*wres) == 0) << 1;
        else *P |= (*opr1 == 0) << 1;
    }
    if (mask & 0x40)
    {
        if (wres)
            *P |= (((((uint8_t)*wres) ^ *opr1) & (((uint8_t)*wres) ^ *opr2) & 0x80) >> 1);
        else *P |= *opr1 & 0x40;
    }
    if (mask & 0x80)
    {
        if (wres) *P |= ((uint8_t)*wres) & 0x80;
        else *P |= *opr1 & 0x80;
    }
}

uint8_t BRANCH_REL (uint16_t *PC, int8_t opr)
{
    uint16_t add = (*PC) + opr;
    uint8_t cycle = (((*PC) >> 8) != (add >> 8));
    *PC = add;
    return 1 + cycle;
}

uint8_t ADC (registers *reg, uint8_t *opr)
{
    uint16_t result = reg->A + *opr + (reg->P & 0x01);
    SET_FLAGS(&(reg->P), 0xC3, &result, &(reg->A), opr);
    reg->A = (uint8_t) result;

    return 1;
}

uint8_t AND (registers *reg, uint8_t *opr)
{
    uint8_t result = reg->A & *opr;
    SET_FLAGS(&(reg->P), 0x82, NULL, &result, NULL);
    reg->A = result;
    return 1;
}

uint8_t ASL (registers *reg, uint8_t *opr)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    uint8_t *ptr = NULL;
    if (opr) ptr = opr;
    else ptr = &(reg->A);

    uint16_t result = ((uint16_t) *ptr) << 1;
    SET_FLAGS(&(reg->P), 0x83, &result, NULL, NULL);
    *ptr = (uint8_t) result;

    return 2;
}

uint8_t BCC (registers *reg, uint8_t *opr)
{
    if (!(reg->P & 0x01))
        return BRANCH_REL(&(reg->PC), (int8_t)*opr);
    return 1;
}

uint8_t BCS (registers *reg, uint8_t *opr)
{
    if (reg->P & 0x01)
        return BRANCH_REL(&(reg->PC), (int8_t)*opr);
    return 1;
}

uint8_t BEQ (registers *reg, uint8_t *opr)
{
    if (reg->P & 0x02)
        return BRANCH_REL(&(reg->PC), (int8_t)*opr);
    return 1;
}

uint8_t BIT (registers *reg, uint8_t *opr)
{
    SET_FLAGS(&(reg->P), 0xC2, NULL, opr, NULL);
    return 1;
}

uint8_t BMI (registers *reg, uint8_t *opr)
{
    if (reg->P & 0x80)
        return BRANCH_REL(&(reg->PC), (int8_t)*opr);
    return 1;
}

uint8_t BNE (registers *reg, uint8_t *opr)
{
    if (!(reg->P & 0x02))
        return BRANCH_REL(&(reg->PC), (int8_t)*opr);
    return 1;
}

uint8_t BPL (registers *reg, uint8_t *opr)
{
    if (!(reg->P & 0x80))
        return BRANCH_REL(&(reg->PC), (int8_t)*opr);
    return 1;
}

uint8_t BRK (registers *reg, uint8_t *opr)
{
    (void) opr;
    reg->P |= 0x04;
    reg->PC = 0xFFFE;
    return 3;
}

uint8_t BVC (registers *reg, uint8_t *opr)
{
    if (!(reg->P & 0x40))
        return BRANCH_REL(&(reg->PC), (int8_t)*opr);
    return 1;
}

uint8_t BVS (registers *reg, uint8_t *opr)
{
    if (reg->P & 0x40)
        return BRANCH_REL(&(reg->PC), (int8_t)*opr);
    return 1;
}

uint8_t CLC (registers *reg, uint8_t *opr)
{
    (void) opr;
    reg->P &= ~(0x01);
    return 2;
}

uint8_t CLD (registers *reg, uint8_t *opr)
{
    (void) opr;
    reg->P &= ~(0x08);
    return 2;
}

uint8_t CLI (registers *reg, uint8_t *opr)
{
    // CLEARING THIS FLAG IS DELAYED BY ONE INSTRUCTION
    // MIGHT NEED TO FIX THE TIMING
    (void) opr;
    reg->P &= ~(0x04);
    return 2;
}

uint8_t CLV (registers *reg, uint8_t *opr)
{
    (void) opr;
    reg->P &= ~(0x40);
    return 2;
}

uint8_t CMP (registers *reg, uint8_t *opr)
{
    uint8_t sub = reg->A - *opr;
    SET_FLAGS(&(reg->P), 0x83, NULL, &sub, NULL);
    return 1;
}
uint8_t CPX (registers *reg, uint8_t *opr)
{
    uint8_t sub = reg->X - *opr;
    SET_FLAGS(&(reg->P), 0x83, NULL, &sub, NULL);
    return 1;
}

uint8_t CPY (registers *reg, uint8_t *opr)
{
    uint8_t sub = reg->Y - *opr;
    SET_FLAGS(&(reg->P), 0x83, NULL, &sub, NULL);
    return 1;
}

uint8_t DEC (registers *reg, uint8_t *opr)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    (*opr)--;
    SET_FLAGS(&(reg->P), 0x82, NULL, opr, NULL);
    return 2;
}

uint8_t DEX (registers *reg, uint8_t *opr)
{
    (void) opr;
    (reg->X)--;
    SET_FLAGS(&(reg->P), 0x82, NULL, &(reg->X), NULL);
    return 2;
}

uint8_t DEY (registers *reg, uint8_t *opr)
{
    (void) opr;
    (reg->Y)--;
    SET_FLAGS(&(reg->P), 0x82, NULL, &(reg->Y), NULL);
    return 2;
}

uint8_t EOR (registers *reg, uint8_t *opr)
{
    uint8_t result = reg->A ^ *opr;
    SET_FLAGS(&(reg->P), 0x82, NULL, &result, NULL);
    reg->A = result;
    return 1;
}

uint8_t INC (registers *reg, uint8_t *opr)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    (*opr)++;
    SET_FLAGS(&(reg->P), 0x82, NULL, opr, NULL);
    return 2;
}

uint8_t INX (registers *reg, uint8_t *opr)
{
    (void) opr;
    (reg->X)++;
    SET_FLAGS(&(reg->P), 0x82, NULL, &(reg->X), NULL);
    return 2;
}

uint8_t INY (registers *reg, uint8_t *opr)
{
    (void) opr;
    (reg->Y)++;
    SET_FLAGS(&(reg->P), 0x82, NULL, &(reg->Y), NULL);
    return 2;
}

uint8_t JMP (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 1;
}

uint8_t JSR (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 1;
}

uint8_t LDA (registers *reg, uint8_t *opr)
{
    SET_FLAGS(&(reg->P), 0x82, NULL, opr, NULL);
    reg->A = *opr;
    return 1;
}

uint8_t LDX (registers *reg, uint8_t *opr)
{
    SET_FLAGS(&(reg->P), 0x82, NULL, opr, NULL);
    reg->X = *opr;
    return 1;
}

uint8_t LDY (registers *reg, uint8_t *opr)
{
    SET_FLAGS(&(reg->P), 0x82, NULL, opr, NULL);
    reg->Y = *opr;
    return 1;
}

uint8_t LSR (registers *reg, uint8_t *opr)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    uint8_t *ptr = NULL;
    if (opr) ptr = opr;
    else ptr = &(reg->A);

    uint16_t result = (((uint16_t) *ptr) << 8) | (((uint16_t)*ptr) >> 1);
    SET_FLAGS(&(reg->P), 0x83, &result, NULL, NULL);
    *ptr = (uint8_t) result;

    return 2;
}

uint8_t NOP (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 2;
}

uint8_t ORA (registers *reg, uint8_t *opr)
{
    uint8_t result = reg->A | *opr;
    SET_FLAGS(&(reg->P), 0x82, NULL, &result, NULL);
    reg->A = result;
    return 1;
}

uint8_t PHA (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 1;
}

uint8_t PHP (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 1;
}

uint8_t PLA (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 1;
}

uint8_t PLP (registers *reg, uint8_t *opr)
{
    // CLEARING THIS FLAG IS DELAYED BY ONE INSTRUCTION
    // MIGHT NEED TO FIX THE TIMING
    (void) reg;
    (void) opr;
    return 1;
}

uint8_t ROL (registers *reg, uint8_t *opr)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    uint8_t *ptr = NULL;
    if (opr) ptr = opr;
    else ptr = &(reg->A);

    uint16_t result = (((uint16_t) *ptr) << 1);
    result |= (result >> 8);
    SET_FLAGS(&(reg->P), 0x83, &result, NULL, NULL);
    *ptr = (uint8_t) result;

    return 2;
}
uint8_t ROR (registers *reg, uint8_t *opr)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    uint8_t *ptr = NULL;
    if (opr) ptr = opr;
    else ptr = &(reg->A);

    uint16_t result = (uint16_t)(((*ptr) >> 1) | ((*ptr) << 7));
    result |= (((uint16_t)(*ptr)) << 8); // Evil workaround for SET_FLAGS
    SET_FLAGS(&(reg->P), 0x83, &result, NULL, NULL);
    *ptr = (uint8_t) result;

    return 2;
}

uint8_t RTI (registers *reg, uint8_t *opr)
{
    // CLEARING THIS FLAG IS DELAYED BY ONE INSTRUCTION
    // MIGHT NEED TO FIX THE TIMING
    (void) reg;
    (void) opr;
    return 1;
}

uint8_t RTS (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 1;
}

uint8_t SBC (registers *reg, uint8_t *opr)
{
    uint8_t tmp = ~(*opr);
    return ADC(reg, &tmp);
}

uint8_t SEC (registers *reg, uint8_t *opr)
{
    (void) opr;
    reg->P |= 0x01;
    return 2;
}

uint8_t SED (registers *reg, uint8_t *opr)
{
    (void) opr;
    reg->P |= 0x08;
    return 2;
}

uint8_t SEI (registers *reg, uint8_t *opr)
{
    // CLEARING THIS FLAG IS DELAYED BY ONE INSTRUCTION
    // MIGHT NEED TO FIX THE TIMING
    (void) opr;
    reg->P |= 0x04;
    return 2;
}

uint8_t STA (registers *reg, uint8_t *opr)
{
    *opr = reg->A;
    return 1;
}

uint8_t STX (registers *reg, uint8_t *opr)
{
    *opr = reg->X;
    return 1;
}

uint8_t STY (registers *reg, uint8_t *opr)
{
    *opr = reg->Y;
    return 1;
}

uint8_t TAX (registers *reg, uint8_t *opr)
{
    (void) opr;
    SET_FLAGS(&(reg->P), 0x82, NULL, &(reg->A), NULL);
    reg->X = reg->A;
    return 2;
}

uint8_t TAY (registers *reg, uint8_t *opr)
{
    (void) opr;
    SET_FLAGS(&(reg->P), 0x82, NULL, &(reg->A), NULL);
    reg->Y = reg->A;
    return 2;
}

uint8_t TSX (registers *reg, uint8_t *opr)
{
    (void) opr;
    SET_FLAGS(&(reg->P), 0x82, NULL, &(reg->S), NULL);
    reg->X = reg->S;
    return 2;
}

uint8_t TXA (registers *reg, uint8_t *opr)
{
    (void) opr;
    SET_FLAGS(&(reg->P), 0x82, NULL, &(reg->X), NULL);
    reg->A = reg->X;
    return 2;
}

uint8_t TXS (registers *reg, uint8_t *opr)
{
    (void) opr;
    SET_FLAGS(&(reg->P), 0x82, NULL, &(reg->X), NULL);
    reg->S = reg->X;
    return 2;
}

uint8_t TYA (registers *reg, uint8_t *opr)
{
    (void) opr;
    SET_FLAGS(&(reg->P), 0x82, NULL, &(reg->A), NULL);
    reg->Y = reg->A;
    return 2;
}

// Unofficial instructions

uint8_t STP (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t SLO (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t RLA (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t SRE (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t RRA (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t ANC (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t ALR (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t ARR (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t SAX (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t AHX (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t XAA (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t TAS (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t SHX (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t SHY (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t LAX (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t DCP (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t ISC (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t LAS (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

uint8_t AXS (registers *reg, uint8_t *opr)
{
    (void) reg;
    (void) opr;
    return 0;
}

const instruction instruction_table[0x100] = 
{
    /*     X0   X1   X2   X3   X4   X5   X6   X7   X8   X9   XA   XB   XC   XD   XE   XF*/
    /*0X*/ BRK, ORA, STP, SLO, NOP, ORA, ASL, SLO, PHP, ORA, ASL, ANC, NOP, ORA, ASL, SLO,
    /*1X*/ BPL, ORA, STP, SLO, NOP, ORA, ASL, SLO, CLC, ORA, NOP, SLO, NOP, ORA, ASL, SLO,
    /*2X*/ JSR, AND, STP, RLA, BIT, AND, ROL, RLA, PLP, AND, ROL, ANC, BIT, AND, ROL, RLA,
    /*3X*/ BMI, AND, STP, RLA, NOP, AND, ROL, RLA, SEC, AND, NOP, RLA, NOP, AND, ROL, RLA,
    /*4X*/ RTI, EOR, STP, SRE, NOP, EOR, LSR, SRE, PHA, EOR, LSR, ALR, JMP, EOR, LSR, SRE,
    /*5X*/ BVC, EOR, STP, SRE, NOP, EOR, LSR, SRE, CLI, EOR, NOP, SRE, NOP, EOR, LSR, SRE,
    /*6X*/ RTS, ADC, STP, RRA, NOP, ADC, ROR, RRA, PLA, ADC, ROR, ARR, JMP, ADC, ROR, RRA,
    /*7X*/ BVS, ADC, STP, RRA, NOP, ADC, ROR, RRA, SEI, ADC, NOP, RRA, NOP, ADC, ROR, RRA,
    /*8X*/ NOP, STA, NOP, SAX, STY, STA, STX, SAX, DEY, NOP, TXA, XAA, STY, STA, STX, SAX,
    /*9X*/ BCC, STA, STP, AHX, STY, STA, STX, SAX, TYA, STA, TXS, TAS, SHY, STA, SHX, AHX,
    /*AX*/ LDY, LDA, LDX, LAX, LDY, LDA, LDX, LAX, TAY, LDA, TAX, LAX, LDY, LDA, LDX, LAX,
    /*BX*/ BCS, LDA, STP, LAX, LDY, LDA, LDX, LAX, CLV, LDA, TSX, LAS, LDY, LDA, LDX, LAX,
    /*CX*/ CPY, CMP, NOP, DCP, CPY, CMP, DEC, DCP, INY, CMP, DEX, AXS, CPY, CMP, DEC, DCP,
    /*DX*/ BNE, CMP, STP, DCP, NOP, CMP, DEC, DCP, CLD, CMP, NOP, DCP, NOP, CMP, DEC, DCP,
    /*EX*/ CPX, SBC, NOP, ISC, CPX, SBC, INC, ISC, INX, SBC, NOP, SBC, CPX, SBC, INC, ISC,
    /*FX*/ BEQ, SBC, STP, ISC, NOP, SBC, INC, ISC, SED, SBC, NOP, ISC, NOP, SBC, INC, ISC
};

const char instruction_name[0x100][4] = 
{
    /*      X0     X1     X2     X3     X4     X5     X6     X7     X8     X9     XA     XB     XC     XD     XE     XF */
    /*0X*/ "BRK", "ORA", "STP", "SLO", "NOP", "ORA", "ASL", "SLO", "PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO",
    /*1X*/ "BPL", "ORA", "STP", "SLO", "NOP", "ORA", "ASL", "SLO", "CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",
    /*2X*/ "JSR", "AND", "STP", "RLA", "BIT", "AND", "ROL", "RLA", "PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA",
    /*3X*/ "BMI", "AND", "STP", "RLA", "NOP", "AND", "ROL", "RLA", "SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",
    /*4X*/ "RTI", "EOR", "STP", "SRE", "NOP", "EOR", "LSR", "SRE", "PHA", "EOR", "LSR", "ALR", "JMP", "EOR", "LSR", "SRE",
    /*5X*/ "BVC", "EOR", "STP", "SRE", "NOP", "EOR", "LSR", "SRE", "CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",
    /*6X*/ "RTS", "ADC", "STP", "RRA", "NOP", "ADC", "ROR", "RRA", "PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
    /*7X*/ "BVS", "ADC", "STP", "RRA", "NOP", "ADC", "ROR", "RRA", "SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",
    /*8X*/ "NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX", "DEY", "NOP", "TXA", "XAA", "STY", "STA", "STX", "SAX",
    /*9X*/ "BCC", "STA", "STP", "AHX", "STY", "STA", "STX", "SAX", "TYA", "STA", "TXS", "TAS", "SHY", "STA", "SHX", "AHX",
    /*AX*/ "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX", "TAY", "LDA", "TAX", "LAX", "LDY", "LDA", "LDX", "LAX",
    /*BX*/ "BCS", "LDA", "STP", "LAX", "LDY", "LDA", "LDX", "LAX", "CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX",
    /*CX*/ "CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP", "INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP",
    /*DX*/ "BNE", "CMP", "STP", "DCP", "NOP", "CMP", "DEC", "DCP", "CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",
    /*EX*/ "CPX", "SBC", "NOP", "ISC", "CPX", "SBC", "INC", "ISC", "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISC",
    /*FX*/ "BEQ", "SBC", "STP", "ISC", "NOP", "SBC", "INC", "ISC", "SED", "SBC", "NOP", "ISC", "NOP", "SBC", "INC", "ISC"
};
