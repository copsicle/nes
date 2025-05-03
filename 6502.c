#include "6502.h"
//#include "memory.h"

#include <stddef.h>
#include <stdint.h>

/*
7  bit  0
---- ----
NV1B DIZC
|||| ||||
|||| |||+- Carry
|||| ||+-- Zero
|||| |+--- Interrupt Disable
|||| +---- Decimal
|||+------ (No CPU effect; see: the B flag)
||+------- (No CPU effect; always pushed as 1)
|+-------- Overflow
+--------- Negative
*/

void SET_FLAGS (uint8_t* P, uint8_t mask, uint16_t* wres, uint8_t* opr1, uint8_t* opr2)
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

uint8_t BRANCH_REL (uint16_t* PC, int8_t opr)
{
    uint16_t add = *PC + opr;
    uint8_t mod = ((*PC / 0x100) != (add / 0x100));
    *PC = add;
    return 2 + mod;
}

uint8_t ADC (struct registers* reg, uint8_t* opr)
{
    uint16_t result = reg->A + *opr + (reg->P & 0x01);
    SET_FLAGS(&reg->P, 0xC3, &result, &reg->A, opr);
    reg->A = (uint8_t) result;

    return 1;
}

uint8_t AND (struct registers* reg, uint8_t* opr)
{
    uint8_t result = reg->A & *opr;
    SET_FLAGS(&reg->P, 0x82, NULL, &result, NULL);
    reg->A = result;
    return 1;
}

uint8_t ASL (struct registers* reg, uint8_t* opr)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    uint8_t* ptr = NULL;
    if (opr) ptr = opr;
    else ptr = &reg->A;

    uint16_t result = 0;
    result = ((uint16_t) *ptr) << 1;
    SET_FLAGS(&reg->P, 0x83, &result, NULL, NULL);
    *ptr = (uint8_t) result;

    return 2;
}

uint8_t BCC (struct registers* reg, uint8_t* opr)
{
    if (!(reg->P & 0x01))
        return BRANCH_REL(&reg->PC, (int8_t)*opr);
    return 1;
}

uint8_t BCS (struct registers* reg, uint8_t* opr)
{
    if (reg->P & 0x01)
        return BRANCH_REL(&reg->PC, (int8_t)*opr);
    return 1;
}

uint8_t BEQ (struct registers* reg, uint8_t* opr)
{
    if (reg->P & 0x02)
        return BRANCH_REL(&reg->PC, (int8_t)*opr);
    return 1;
}

uint8_t BIT (struct registers* reg, uint8_t* opr)
{
    SET_FLAGS(&reg->P, 0xC2, NULL, opr, NULL);
    return 1;
}

uint8_t BMI (struct registers* reg, uint8_t* opr)
{
    if (reg->P & 0x80)
        return BRANCH_REL(&reg->PC, (int8_t)*opr);
    return 1;
}

uint8_t BNE (struct registers* reg, uint8_t* opr)
{
    if (!(reg->P & 0x02))
        return BRANCH_REL(&reg->PC, (int8_t)*opr);
    return 1;
}

uint8_t BPL (struct registers* reg, uint8_t* opr)
{
    if (!(reg->P & 0x80))
        return BRANCH_REL(&reg->PC, (int8_t)*opr);
    return 1;
}

uint8_t BRK (struct registers* reg, uint8_t* opr)
{
    reg->P |= 0x04;
    reg->PC = 0xFFFE;
    return 3;
}

uint8_t BVC (struct registers* reg, uint8_t* opr)
{
    if (!(reg->P & 0x40))
        return BRANCH_REL(&reg->PC, (int8_t)*opr);
    return 1;
}

uint8_t BVS (struct registers* reg, uint8_t* opr)
{
    if (reg->P & 0x40)
        return BRANCH_REL(&reg->PC, (int8_t)*opr);
    return 1;
}

uint8_t CLC (struct registers* reg, uint8_t* opr)
{
    reg->P &= ~(0x01);
    return 2;
}

uint8_t CLD (struct registers* reg, uint8_t* opr)
{
    reg->P &= ~(0x08);
    return 2;
}

uint8_t CLI (struct registers* reg, uint8_t* opr)
{
    reg->P &= ~(0x04);
    return 2;
}

uint8_t CLV (struct registers* reg, uint8_t* opr)
{
    reg->P &= ~(0x40);
    return 2;
}

uint8_t CMP (struct registers* reg, uint8_t* opr)
{
    uint8_t sub = reg->A - *opr;
    SET_FLAGS(&reg->P, 0x83, NULL, &sub, NULL);
    return 1;
}
uint8_t CPX (struct registers* reg, uint8_t* opr)
{
    uint8_t sub = reg->X - *opr;
    SET_FLAGS(&reg->P, 0x83, NULL, &sub, NULL);
    return 1;
}

uint8_t CPY (struct registers* reg, uint8_t* opr)
{
    uint8_t sub = reg->Y - *opr;
    SET_FLAGS(&reg->P, 0x83, NULL, &sub, NULL);
    return 1;
}

uint8_t DEC (struct registers* reg, uint8_t* opr)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    (*opr)--;
    SET_FLAGS(&reg->P, 0x82, NULL, opr, NULL);
    return 2;
}

uint8_t DEX (struct registers* reg, uint8_t* opr)
{
    (reg->X)--;
    SET_FLAGS(&reg->P, 0x82, NULL, &reg->X, NULL);
    return 2;
}

uint8_t DEY (struct registers* reg, uint8_t* opr)
{
    (reg->Y)--;
    SET_FLAGS(&reg->P, 0x82, NULL, &reg->Y, NULL);
    return 2;
}

uint8_t EOR (struct registers* reg, uint8_t* opr)
{
    uint8_t result = reg->A ^ *opr;
    SET_FLAGS(&reg->P, 0x82, NULL, &result, NULL);
    reg->A = result;
    return 1;
}

uint8_t INC (struct registers* reg, uint8_t* opr)
{
    // THIS IS A DUMMY WRITING FUNCTION WHICH MIGHT NEED TIMING FIXES
    (*opr)++;
    SET_FLAGS(&reg->P, 0x82, NULL, opr, NULL);
    return 2;
}

uint8_t INX (struct registers* reg, uint8_t* opr)
{
    (reg->X)++;
    SET_FLAGS(&reg->P, 0x82, NULL, &reg->X, NULL);
    return 2;
}

uint8_t INY (struct registers* reg, uint8_t* opr)
{
    (reg->Y)++;
    SET_FLAGS(&reg->P, 0x82, NULL, &reg->Y, NULL);
    return 2;
}

uint8_t JMP (struct registers* reg, uint8_t* opr)
{
    return 1;
}