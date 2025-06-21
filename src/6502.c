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

void RESET_CPU (nes con)
{
    con->CPU->PC = 0xFFFC;
    con->CPU->S -= 3;
    con->CPU->P |= 0x24;
}

void PRINT_CPU (nes con)
{
    printf("CPU Dump:\n");
    printf("PC: 0x%04X | ", con->CPU->PC);
    printf("SP: 0x%02X (%d)\n", con->CPU->S, con->CPU->S);
    printf("A: 0x%02X (%d) | ", con->CPU->A, con->CPU->A);
    printf("X: 0x%02X (%d) | ", con->CPU->X, con->CPU->X);
    printf("Y: 0x%02X (%d)\n", con->CPU->A, con->CPU->A);
    printf("FLAGS: 0x%02X\n\n", con->CPU->P);
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

uint8_t ADC (nes con)
{
    uint16_t result = con->CPU->A + *con->CPU->OPR + (con->CPU->P & 0x01);
    SET_FLAGS(&(con->CPU->P), 0xC3, &result, &(con->CPU->A), con->CPU->OPR);
    con->CPU->A = (uint8_t) result;

    return 1;
}

uint8_t AND (nes con)
{
    uint8_t result = con->CPU->A & *con->CPU->OPR;
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &result, NULL);
    con->CPU->A = result;
    return 1;
}

uint8_t ASL (nes con)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    uint8_t *ptr = NULL;
    if (&(*con->CPU->OPR)) ptr = &(*con->CPU->OPR);
    else ptr = &(con->CPU->A);

    uint16_t result = ((uint16_t) *ptr) << 1;
    SET_FLAGS(&(con->CPU->P), 0x83, &result, NULL, NULL);
    *ptr = (uint8_t) result;

    return 2;
}

uint8_t BCC (nes con)
{
    if (!(con->CPU->P & 0x01))
        return BRANCH_REL(&(con->CPU->PC), (int8_t)*con->CPU->OPR);
    return 1;
}

uint8_t BCS (nes con)
{
    if (con->CPU->P & 0x01)
        return BRANCH_REL(&(con->CPU->PC), (int8_t)*con->CPU->OPR);
    return 1;
}

uint8_t BEQ (nes con)
{
    if (con->CPU->P & 0x02)
        return BRANCH_REL(&(con->CPU->PC), (int8_t)*con->CPU->OPR);
    return 1;
}

uint8_t BIT (nes con)
{
    SET_FLAGS(&(con->CPU->P), 0xC2, NULL, &(*con->CPU->OPR), NULL);
    return 1;
}

uint8_t BMI (nes con)
{
    if (con->CPU->P & 0x80)
        return BRANCH_REL(&(con->CPU->PC), (int8_t)*con->CPU->OPR);
    return 1;
}

uint8_t BNE (nes con)
{
    if (!(con->CPU->P & 0x02))
        return BRANCH_REL(&(con->CPU->PC), (int8_t)*con->CPU->OPR);
    return 1;
}

uint8_t BPL (nes con)
{
    if (!(con->CPU->P & 0x80))
        return BRANCH_REL(&(con->CPU->PC), (int8_t)*con->CPU->OPR);
    return 1;
}

uint8_t BRK (nes con)
{
    con->CPU->P |= 0x04;
    con->CPU->PC = 0xFFFE;
    return 3;
}

uint8_t BVC (nes con)
{
    if (!(con->CPU->P & 0x40))
        return BRANCH_REL(&(con->CPU->PC), (int8_t)*con->CPU->OPR);
    return 1;
}

uint8_t BVS (nes con)
{
    if (con->CPU->P & 0x40)
        return BRANCH_REL(&(con->CPU->PC), (int8_t)*con->CPU->OPR);
    return 1;
}

uint8_t CLC (nes con)
{
    con->CPU->P &= ~(0x01);
    return 2;
}

uint8_t CLD (nes con)
{
    con->CPU->P &= ~(0x08);
    return 2;
}

uint8_t CLI (nes con)
{
    // CLEARING THIS FLAG IS DELAYED BY ONE INSTRUCTION
    // MIGHT NEED TO FIX THE TIMING
    con->CPU->P &= ~(0x04);
    return 2;
}

uint8_t CLV (nes con)
{
    con->CPU->P &= ~(0x40);
    return 2;
}

uint8_t CMP (nes con)
{
    uint8_t sub = con->CPU->A - *con->CPU->OPR;
    SET_FLAGS(&(con->CPU->P), 0x83, NULL, &sub, NULL);
    return 1;
}
uint8_t CPX (nes con)
{
    uint8_t sub = con->CPU->X - *con->CPU->OPR;
    SET_FLAGS(&(con->CPU->P), 0x83, NULL, &sub, NULL);
    return 1;
}

uint8_t CPY (nes con)
{
    uint8_t sub = con->CPU->Y - *con->CPU->OPR;
    SET_FLAGS(&(con->CPU->P), 0x83, NULL, &sub, NULL);
    return 1;
}

uint8_t DEC (nes con)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    (*con->CPU->OPR)--;
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(*con->CPU->OPR), NULL);
    return 2;
}

uint8_t DEX (nes con)
{
    (con->CPU->X)--;
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(con->CPU->X), NULL);
    return 2;
}

uint8_t DEY (nes con)
{
    (con->CPU->Y)--;
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(con->CPU->Y), NULL);
    return 2;
}

uint8_t EOR (nes con)
{
    uint8_t result = con->CPU->A ^ *con->CPU->OPR;
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &result, NULL);
    con->CPU->A = result;
    return 1;
}

uint8_t INC (nes con)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    (*con->CPU->OPR)++;
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(*con->CPU->OPR), NULL);
    return 2;
}

uint8_t INX (nes con)
{
    (con->CPU->X)++;
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(con->CPU->X), NULL);
    return 2;
}

uint8_t INY (nes con)
{
    (con->CPU->Y)++;
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(con->CPU->Y), NULL);
    return 2;
}

uint8_t JMP (nes con)
{
    (void) con;
    return 1;
}

uint8_t JSR (nes con)
{
    (void) con;
    return 1;
}

uint8_t LDA (nes con)
{
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(*con->CPU->OPR), NULL);
    con->CPU->A = *con->CPU->OPR;
    return 1;
}

uint8_t LDX (nes con)
{
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(*con->CPU->OPR), NULL);
    con->CPU->X = *con->CPU->OPR;
    return 1;
}

uint8_t LDY (nes con)
{
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(*con->CPU->OPR), NULL);
    con->CPU->Y = *con->CPU->OPR;
    return 1;
}

uint8_t LSR (nes con)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    uint8_t *ptr = NULL;
    if (&(*con->CPU->OPR)) ptr = &(*con->CPU->OPR);
    else ptr = &(con->CPU->A);

    uint16_t result = (((uint16_t) *ptr) << 8) | (((uint16_t)*ptr) >> 1);
    SET_FLAGS(&(con->CPU->P), 0x83, &result, NULL, NULL);
    *ptr = (uint8_t) result;

    return 2;
}

uint8_t NOP (nes con)
{
    (void) con;
    return 2;
}

uint8_t ORA (nes con)
{
    uint8_t result = con->CPU->A | *con->CPU->OPR;
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &result, NULL);
    con->CPU->A = result;
    return 1;
}

uint8_t PHA (nes con)
{
    (void) con;
    return 1;
}

uint8_t PHP (nes con)
{
    (void) con;
    return 1;
}

uint8_t PLA (nes con)
{
    (void) con;
    return 1;
}

uint8_t PLP (nes con)
{
    // CLEARING THIS FLAG IS DELAYED BY ONE INSTRUCTION
    // MIGHT NEED TO FIX THE TIMING
    (void) con;
    return 1;
}

uint8_t ROL (nes con)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    uint8_t *ptr = NULL;
    if (&(*con->CPU->OPR)) ptr = &(*con->CPU->OPR);
    else ptr = &(con->CPU->A);

    uint16_t result = (((uint16_t) *ptr) << 1);
    result |= (result >> 8);
    SET_FLAGS(&(con->CPU->P), 0x83, &result, NULL, NULL);
    *ptr = (uint8_t) result;

    return 2;
}
uint8_t ROR (nes con)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    uint8_t *ptr = NULL;
    if (&(*con->CPU->OPR)) ptr = &(*con->CPU->OPR);
    else ptr = &(con->CPU->A);

    uint16_t result = (uint16_t)(((*ptr) >> 1) | ((*ptr) << 7));
    result |= (((uint16_t)(*ptr)) << 8); // Evil workaround for SET_FLAGS
    SET_FLAGS(&(con->CPU->P), 0x83, &result, NULL, NULL);
    *ptr = (uint8_t) result;

    return 2;
}

uint8_t RTI (nes con)
{
    // CLEARING THIS FLAG IS DELAYED BY ONE INSTRUCTION
    // MIGHT NEED TO FIX THE TIMING
    (void) con;
    return 1;
}

uint8_t RTS (nes con)
{
    (void) con;
    return 1;
}

uint8_t SBC (nes con)
{
    *con->CPU->OPR = ~(*con->CPU->OPR);
    return ADC(con);
}

uint8_t SEC (nes con)
{
    con->CPU->P |= 0x01;
    return 2;
}

uint8_t SED (nes con)
{
    con->CPU->P |= 0x08;
    return 2;
}

uint8_t SEI (nes con)
{
    // CLEARING THIS FLAG IS DELAYED BY ONE INSTRUCTION
    // MIGHT NEED TO FIX THE TIMING
    con->CPU->P |= 0x04;
    return 2;
}

uint8_t STA (nes con)
{
    *con->CPU->OPR = con->CPU->A;
    return 1;
}

uint8_t STX (nes con)
{
    *con->CPU->OPR = con->CPU->X;
    return 1;
}

uint8_t STY (nes con)
{
    *con->CPU->OPR = con->CPU->Y;
    return 1;
}

uint8_t TAX (nes con)
{
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(con->CPU->A), NULL);
    con->CPU->X = con->CPU->A;
    return 2;
}

uint8_t TAY (nes con)
{
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(con->CPU->A), NULL);
    con->CPU->Y = con->CPU->A;
    return 2;
}

uint8_t TSX (nes con)
{
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(con->CPU->S), NULL);
    con->CPU->X = con->CPU->S;
    return 2;
}

uint8_t TXA (nes con)
{
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(con->CPU->X), NULL);
    con->CPU->A = con->CPU->X;
    return 2;
}

uint8_t TXS (nes con)
{
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(con->CPU->X), NULL);
    con->CPU->S = con->CPU->X;
    return 2;
}

uint8_t TYA (nes con)
{
    SET_FLAGS(&(con->CPU->P), 0x82, NULL, &(con->CPU->A), NULL);
    con->CPU->Y = con->CPU->A;
    return 2;
}

// Unofficial instructions

uint8_t STP (nes con)
{
    (void) con;
    return 0;
}

uint8_t SLO (nes con)
{
    (void) con;
    return 0;
}

uint8_t RLA (nes con)
{
    (void) con;
    return 0;
}

uint8_t SRE (nes con)
{
    (void) con;
    return 0;
}

uint8_t RRA (nes con)
{
    (void) con;
    return 0;
}

uint8_t ANC (nes con)
{
    (void) con;
    return 0;
}

uint8_t ALR (nes con)
{
    (void) con;
    return 0;
}

uint8_t ARR (nes con)
{
    (void) con;
    return 0;
}

uint8_t SAX (nes con)
{
    (void) con;
    return 0;
}

uint8_t AHX (nes con)
{
    (void) con;
    return 0;
}

uint8_t XAA (nes con)
{
    (void) con;
    return 0;
}

uint8_t TAS (nes con)
{
    (void) con;
    return 0;
}

uint8_t SHX (nes con)
{
    (void) con;
    return 0;
}

uint8_t SHY (nes con)
{
    (void) con;
    return 0;
}

uint8_t LAX (nes con)
{
    (void) con;
    return 0;
}

uint8_t DCP (nes con)
{
    (void) con;
    return 0;
}

uint8_t ISC (nes con)
{
    (void) con;
    return 0;
}

uint8_t LAS (nes con)
{
    (void) con;
    return 0;
}

uint8_t AXS (nes con)
{
    (void) con;
    return 0;
}

const operation instruction_table[0x100] = 
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
