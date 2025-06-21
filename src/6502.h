#ifndef _6502_H
#define _6502_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define CPU_NTSC 12
#define CPU_PAL 16

#include "types.h"

void RESET_CPU (nes con);
void PRINT_CPU (nes con);

extern const operation instruction_table[0x100];
extern const char instruction_name[0x100][4];


#endif
