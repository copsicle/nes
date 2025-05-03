#ifndef _6502_H
#define _6502_H

#include <stdint.h>

struct registers
{
    uint8_t A; // Accumulator
    uint8_t X; // X register
    uint8_t Y; // Y register
    uint16_t PC; // Program counter
    uint8_t S; // Stack Pointer
    uint8_t P; // Status Register

    //uint8_t OP; // NOT AN OFFICIAL REGISTER, FOR INTERNAL USE
};

// Official instructions
uint8_t ADC (struct registers* reg, uint8_t* opr);
uint8_t AND (struct registers* reg, uint8_t* opr);
uint8_t ASL (struct registers* reg, uint8_t* opr);
uint8_t BCC (struct registers* reg, uint8_t* opr);
uint8_t BCS (struct registers* reg, uint8_t* opr);
uint8_t BEQ (struct registers* reg, uint8_t* opr);
uint8_t BIT (struct registers* reg, uint8_t* opr);
uint8_t BMI (struct registers* reg, uint8_t* opr);
uint8_t BNE (struct registers* reg, uint8_t* opr);
uint8_t BPL (struct registers* reg, uint8_t* opr);
uint8_t BRK (struct registers* reg, uint8_t* opr);
uint8_t BVC (struct registers* reg, uint8_t* opr);
uint8_t BVS (struct registers* reg, uint8_t* opr);
uint8_t CLC (struct registers* reg, uint8_t* opr);
uint8_t CLD (struct registers* reg, uint8_t* opr);
uint8_t CLI (struct registers* reg, uint8_t* opr);
uint8_t CLV (struct registers* reg, uint8_t* opr);
uint8_t CMP (struct registers* reg, uint8_t* opr);
uint8_t ADC (struct registers* reg, uint8_t* opr);
uint8_t AND (struct registers* reg, uint8_t* opr);
uint8_t ASL (struct registers* reg, uint8_t* opr);
uint8_t BCC (struct registers* reg, uint8_t* opr);
uint8_t BCS (struct registers* reg, uint8_t* opr);
uint8_t BEQ (struct registers* reg, uint8_t* opr);
uint8_t BIT (struct registers* reg, uint8_t* opr);
uint8_t BMI (struct registers* reg, uint8_t* opr);
uint8_t BNE (struct registers* reg, uint8_t* opr);
uint8_t BPL (struct registers* reg, uint8_t* opr);
uint8_t BRK (struct registers* reg, uint8_t* opr);
uint8_t BVC (struct registers* reg, uint8_t* opr);
uint8_t BVS (struct registers* reg, uint8_t* opr);
uint8_t CLC (struct registers* reg, uint8_t* opr);
uint8_t CLD (struct registers* reg, uint8_t* opr);
uint8_t CLI (struct registers* reg, uint8_t* opr);
uint8_t CLV (struct registers* reg, uint8_t* opr);
uint8_t CMP (struct registers* reg, uint8_t* opr);
uint8_t CPX (struct registers* reg, uint8_t* opr);
uint8_t CPY (struct registers* reg, uint8_t* opr);
uint8_t DEC (struct registers* reg, uint8_t* opr);
uint8_t DEX (struct registers* reg, uint8_t* opr);
uint8_t DEY (struct registers* reg, uint8_t* opr);
uint8_t EOR (struct registers* reg, uint8_t* opr);
uint8_t INC (struct registers* reg, uint8_t* opr);
uint8_t INX (struct registers* reg, uint8_t* opr);
uint8_t INY (struct registers* reg, uint8_t* opr);
uint8_t JMP (struct registers* reg, uint8_t* opr);
uint8_t JSR (struct registers* reg, uint8_t* opr);
uint8_t LDA (struct registers* reg, uint8_t* opr);
uint8_t LDX (struct registers* reg, uint8_t* opr);
uint8_t LDY (struct registers* reg, uint8_t* opr);
uint8_t LSR (struct registers* reg, uint8_t* opr);
uint8_t NOP (struct registers* reg, uint8_t* opr);
uint8_t ORA (struct registers* reg, uint8_t* opr);
uint8_t PHA (struct registers* reg, uint8_t* opr);
uint8_t PHP (struct registers* reg, uint8_t* opr);
uint8_t PLA (struct registers* reg, uint8_t* opr);
uint8_t PLP (struct registers* reg, uint8_t* opr);
uint8_t ROL (struct registers* reg, uint8_t* opr);
uint8_t ROR (struct registers* reg, uint8_t* opr);
uint8_t RTI (struct registers* reg, uint8_t* opr);
uint8_t RTS (struct registers* reg, uint8_t* opr);
uint8_t SBC (struct registers* reg, uint8_t* opr);
uint8_t SEC (struct registers* reg, uint8_t* opr);
uint8_t SED (struct registers* reg, uint8_t* opr);
uint8_t SEI (struct registers* reg, uint8_t* opr);
uint8_t STA (struct registers* reg, uint8_t* opr);
uint8_t STX (struct registers* reg, uint8_t* opr);
uint8_t STY (struct registers* reg, uint8_t* opr);
uint8_t TAX (struct registers* reg, uint8_t* opr);
uint8_t TAY (struct registers* reg, uint8_t* opr);
uint8_t TSX (struct registers* reg, uint8_t* opr);
uint8_t TXA (struct registers* reg, uint8_t* opr);
uint8_t TXS (struct registers* reg, uint8_t* opr);
uint8_t TYA (struct registers* reg, uint8_t* opr);

// Unofficial instructions
uint8_t STP (struct registers* reg, uint8_t* opr);
uint8_t SLO (struct registers* reg, uint8_t* opr);
uint8_t RLA (struct registers* reg, uint8_t* opr);
uint8_t SRE (struct registers* reg, uint8_t* opr);
uint8_t RRA (struct registers* reg, uint8_t* opr);
uint8_t ANC (struct registers* reg, uint8_t* opr);
uint8_t ALR (struct registers* reg, uint8_t* opr);
uint8_t ARR (struct registers* reg, uint8_t* opr);
uint8_t SAX (struct registers* reg, uint8_t* opr);
uint8_t AHX (struct registers* reg, uint8_t* opr);
uint8_t XAA (struct registers* reg, uint8_t* opr);
uint8_t TAS (struct registers* reg, uint8_t* opr);
uint8_t SHX (struct registers* reg, uint8_t* opr);
uint8_t SHY (struct registers* reg, uint8_t* opr);
uint8_t LAX (struct registers* reg, uint8_t* opr);
uint8_t DCP (struct registers* reg, uint8_t* opr);
uint8_t ISC (struct registers* reg, uint8_t* opr);
uint8_t LAS (struct registers* reg, uint8_t* opr);
uint8_t AXS (struct registers* reg, uint8_t* opr);

typedef uint8_t (*instruction_ptr)(struct registers*, uint8_t*);

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