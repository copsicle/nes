//#include <time.h>
//#include <windows.h>

#include "types.h"
#include "6502.h"
#include "ppu.h"
#include "memory.h"
#include "nes2.h"
#include "main.h"

void DEBUG_CPU (registers *reg, operation *op, uint8_t phase, uint8_t *opr)
{
    switch (phase)
    {
        case 0:
            PRINT_CPU(reg);
            break;
        case 1:
            printf("Start processing instruction: %s (0x%02X)\n\n",
                instruction_name[*opr], *opr);
            break;
        case 2:
            if (opr && *op)
                printf("Operand is 0x%02X (%d)\n\n", *opr, *opr);
            else printf("No operand (implied)\n\n");
            break;
        default:
            printf("Bad CPU phase!\n\n");
    }
}

uint8_t CPU (registers *reg, memory *mem, cartridge *cart, uint8_t *phase, operation *op, instruction *inst, uint8_t **opr)
{
    switch (*phase)
    {
        case 0:
            *phase = 1;
            *opr = NULL;
            uint8_t cycle = IMM(reg, mem, cart, opr);
            *op = operation_table[**opr];
            *inst = instruction_table[**opr];
            return cycle;
        case 1:
            *phase = 2;
            if (*op)
                return (*op)(reg, mem, cart, opr);
            __attribute__((fallthrough));
        case 2:
            *phase = 0;
            if (*inst)
                return (*inst)(reg, *opr);
            return 0;
        default:
            *phase = 0;
            return 0;
    }
}

/*
MASTER CLOCK CYCLE

PPU DOT - IF 4/5 MASTER CLOCK CYCLES HAVE PASSED
CPU CYCLE - IF 12/16 MASTER CLOCK CYCLES HAVE PASSED

CPU CYCLE

IF A MEMORY OPERATION/CPU INSTRUCTION TAKES MORE THAN 1 CYCLE,
IT WOULD BE DONE AT THE SPOT BUT THE NEXT CYCLES WOULD BE SKIPPED UNTIL ITS "DONE"
*/

void LOOP (registers *reg, memory *mem, cartridge *cart, ppu* gpu)
{
    uint8_t *opr = NULL;
    uint8_t cpucycle = JMPB(reg, mem, cart, &opr);
    uint8_t phase = 0;
    printf("Initial jump to address: 0x%04X\n\n", reg->PC);
    operation memptr;
    instruction insptr;

    for (uint8_t clock = 0; 1; clock++)
    {
        if (clock == MAX_CLOCK)
            clock = 0;
        
        if ((clock % reg->C) == 0)
        {
            if (cpucycle > 0)
                cpucycle--;
            else
            {
                cpucycle = CPU(reg, mem, cart, &phase, &memptr, &insptr, &opr);
                DEBUG_CPU(reg, &memptr, phase, opr);
            }
        }

        if ((clock % gpu->C) == 0)
        {
            // DO PPU CYCLE
        }
    }
}

int main (int argc, char *argv[])
{
    (void) argc;
    (void) argv;
    
    registers reg = {0};
    RESET_CPU(&reg);
    memory mem = {0};
    if (INIT_MEM(&mem)) return 1;
    ppu gpu = {0};

    FILE *rom = fopen("roms/test.nes", "rb");
    if (!rom) return 1;
    nesheader head = {0};
    cartridge cart = {0};
    if (LOAD_ROM(rom, &head, &mem, &cart))
        return 1;
    if (head.TIMING == 1)
    {
            reg.C = CPU_PAL;
            gpu.C = PPU_PAL;
    }
    else
    {
        reg.C = CPU_NTSC;
        gpu.C = PPU_NTSC;
    }
    
    LOOP(&reg, &mem, &cart, &gpu);

    FREE_MEM(&mem);
    fclose(rom);
    return 0;
}
