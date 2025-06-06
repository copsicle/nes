#ifndef _6502_H
#define _6502_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define CPU_NTSC 12
#define CPU_PAL 16

struct registers;

void RESET_CPU (registers *reg);
void PRINT_CPU (registers *reg);

typedef uint8_t (*instruction)(registers*, uint8_t*);

extern const instruction instruction_table[0x100];
extern const char instruction_name[0x100][4];


#endif
