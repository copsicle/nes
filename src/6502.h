#ifndef _6502_H
#define _6502_H

#include <stdlib.h>
#include <stdint.h>

#define NTSC_CYCLE 559
#define PAL_CYCLE 601

typedef struct
{
    uint8_t A; // Accumulator
    uint8_t X; // X register
    uint8_t Y; // Y register
    uint16_t PC; // Program counter
    uint8_t S; // Stack Pointer
    uint8_t P; // Status Register
    
    uint16_t C; // Not a register, rather the clock freq
} registers;

void RESET_CPU (registers* reg);

typedef uint8_t (*instruction)(registers*, uint8_t*);

extern const instruction instruction_table[0x100];

#endif
