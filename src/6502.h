#ifndef _6502_H
#define _6502_H

#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    uint8_t A; // Accumulator
    uint8_t X; // X register
    uint8_t Y; // Y register
    uint16_t PC; // Program counter
    uint8_t S; // Stack Pointer
    uint8_t P; // Status Register
} registers;

// Official instructions
uint8_t ADC (registers* reg, uint8_t* opr);
uint8_t AND (registers* reg, uint8_t* opr);
uint8_t ASL (registers* reg, uint8_t* opr);
uint8_t BCC (registers* reg, uint8_t* opr);
uint8_t BCS (registers* reg, uint8_t* opr);
uint8_t BEQ (registers* reg, uint8_t* opr);
uint8_t BIT (registers* reg, uint8_t* opr);
uint8_t BMI (registers* reg, uint8_t* opr);
uint8_t BNE (registers* reg, uint8_t* opr);
uint8_t BPL (registers* reg, uint8_t* opr);
uint8_t BRK (registers* reg, uint8_t* opr);
uint8_t BVC (registers* reg, uint8_t* opr);
uint8_t BVS (registers* reg, uint8_t* opr);
uint8_t CLC (registers* reg, uint8_t* opr);
uint8_t CLD (registers* reg, uint8_t* opr);
uint8_t CLI (registers* reg, uint8_t* opr);
uint8_t CLV (registers* reg, uint8_t* opr);
uint8_t CMP (registers* reg, uint8_t* opr);
uint8_t CPX (registers* reg, uint8_t* opr);
uint8_t CPY (registers* reg, uint8_t* opr);
uint8_t DEC (registers* reg, uint8_t* opr);
uint8_t DEX (registers* reg, uint8_t* opr);
uint8_t DEY (registers* reg, uint8_t* opr);
uint8_t EOR (registers* reg, uint8_t* opr);
uint8_t INC (registers* reg, uint8_t* opr);
uint8_t INX (registers* reg, uint8_t* opr);
uint8_t INY (registers* reg, uint8_t* opr);
uint8_t JMP (registers* reg, uint8_t* opr);
uint8_t JSR (registers* reg, uint8_t* opr);
uint8_t LDA (registers* reg, uint8_t* opr);
uint8_t LDX (registers* reg, uint8_t* opr);
uint8_t LDY (registers* reg, uint8_t* opr);
uint8_t LSR (registers* reg, uint8_t* opr);
uint8_t NOP (registers* reg, uint8_t* opr);
uint8_t ORA (registers* reg, uint8_t* opr);
uint8_t PHA (registers* reg, uint8_t* opr);
uint8_t PHP (registers* reg, uint8_t* opr);
uint8_t PLA (registers* reg, uint8_t* opr);
uint8_t PLP (registers* reg, uint8_t* opr);
uint8_t ROL (registers* reg, uint8_t* opr);
uint8_t ROR (registers* reg, uint8_t* opr);
uint8_t RTI (registers* reg, uint8_t* opr);
uint8_t RTS (registers* reg, uint8_t* opr);
uint8_t SBC (registers* reg, uint8_t* opr);
uint8_t SEC (registers* reg, uint8_t* opr);
uint8_t SED (registers* reg, uint8_t* opr);
uint8_t SEI (registers* reg, uint8_t* opr);
uint8_t STA (registers* reg, uint8_t* opr);
uint8_t STX (registers* reg, uint8_t* opr);
uint8_t STY (registers* reg, uint8_t* opr);
uint8_t TAX (registers* reg, uint8_t* opr);
uint8_t TAY (registers* reg, uint8_t* opr);
uint8_t TSX (registers* reg, uint8_t* opr);
uint8_t TXA (registers* reg, uint8_t* opr);
uint8_t TXS (registers* reg, uint8_t* opr);
uint8_t TYA (registers* reg, uint8_t* opr);

// Unofficial instructions
uint8_t STP (registers* reg, uint8_t* opr);
uint8_t SLO (registers* reg, uint8_t* opr);
uint8_t RLA (registers* reg, uint8_t* opr);
uint8_t SRE (registers* reg, uint8_t* opr);
uint8_t RRA (registers* reg, uint8_t* opr);
uint8_t ANC (registers* reg, uint8_t* opr);
uint8_t ALR (registers* reg, uint8_t* opr);
uint8_t ARR (registers* reg, uint8_t* opr);
uint8_t SAX (registers* reg, uint8_t* opr);
uint8_t AHX (registers* reg, uint8_t* opr);
uint8_t XAA (registers* reg, uint8_t* opr);
uint8_t TAS (registers* reg, uint8_t* opr);
uint8_t SHX (registers* reg, uint8_t* opr);
uint8_t SHY (registers* reg, uint8_t* opr);
uint8_t LAX (registers* reg, uint8_t* opr);
uint8_t DCP (registers* reg, uint8_t* opr);
uint8_t ISC (registers* reg, uint8_t* opr);
uint8_t LAS (registers* reg, uint8_t* opr);
uint8_t AXS (registers* reg, uint8_t* opr);

typedef uint8_t (*instruction_ptr)(registers*, uint8_t*);

static instruction_ptr instructions[0x100] = 
{
    //     X0   X1   X2   X3   X4   X5   X6   X7   X8   X9   XA   XB   XC   XD   XE   XF
    /*00*/ BRK, ORA, STP, SLO, NOP, ORA, ASL, SLO, PHP, ORA, ASL, ANC, NOP, ORA, ASL, SLO,
    /*10*/ BPL, ORA, STP, SLO, NOP, ORA, ASL, SLO, CLC, ORA, NOP, SLO, NOP, ORA, ASL, SLO,
    /*20*/ JSR, AND, STP, RLA, BIT, AND, ROL, RLA, PLP, AND, ROL, ANC, BIT, AND, ROL, RLA,
    /*30*/ BMI, AND, STP, RLA, NOP, AND, ROL, RLA, SEC, AND, NOP, RLA, NOP, AND, ROL, RLA,
    /*40*/ RTI, EOR, STP, SRE, NOP, EOR, LSR, SRE, PHA, EOR, LSR, ALR, JMP, EOR, LSR, SRE,
    /*50*/ BVC, EOR, STP, SRE, NOP, EOR, LSR, SRE, CLI, EOR, NOP, SRE, NOP, EOR, LSR, SRE,
    /*60*/ RTS, ADC, STP, RRA, NOP, ADC, ROR, RRA, PLA, ADC, ROR, ARR, JMP, ADC, ROR, RRA,
    /*70*/ BVS, ADC, STP, RRA, NOP, ADC, ROR, RRA, SEI, ADC, NOP, RRA, NOP, ADC, ROR, RRA,
    /*80*/ NOP, STA, NOP, SAX, STY, STA, STX, SAX, DEY, NOP, TXA, XAA, STY, STA, STX, SAX,
    /*90*/ BCC, STA, STP, AHX, STY, STA, STX, SAX, TYA, STA, TXS, TAS, SHY, STA, SHX, AHX,
    /*A0*/ LDY, LDA, LDX, LAX, LDY, LDA, LDX, LAX, TAY, LDA, TAX, LAX, LDY, LDA, LDX, LAX,
    /*B0*/ BCS, LDA, STP, LAX, LDY, LDA, LDX, LAX, CLV, LDA, TSX, LAS, LDY, LDA, LDX, LAX,
    /*C0*/ CPY, CMP, NOP, DCP, CPY, CMP, DEC, DCP, INY, CMP, DEX, AXS, CPY, CMP, DEC, DCP,
    /*D0*/ BNE, CMP, STP, DCP, NOP, CMP, DEC, DCP, CLD, CMP, NOP, DCP, NOP, CMP, DEC, DCP,
    /*E0*/ CPX, SBC, NOP, ISC, CPX, SBC, INC, ISC, INX, SBC, NOP, SBC, CPX, SBC, INC, ISC,
    /*F0*/ BEQ, SBC, STP, ISC, NOP, SBC, INC, ISC, SED, SBC, NOP, ISC, NOP, SBC, INC, ISC
};

#endif