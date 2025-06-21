//#include <time.h>
//#include <windows.h>

#include "types.h"
#include "6502.h"
#include "ppu.h"
#include "memory.h"
#include "nes2.h"
#include "main.h"

void DEBUG_CPU (nes con, operation *op)
{
    uint8_t *opr = con->CPU->OPR;
    switch (con->CPU->PHS)
    {
        case FETCH:
            PRINT_CPU(con);
            break;
        case MEMOP:
            printf("Start processing instruction: %s (0x%02X)\n\n",
                instruction_name[*opr], *opr);
            break;
        case WORK:
            if (con->CPU->OPR && *op)
                printf("Operand is 0x%02X (%d)\n\n", *opr, *opr);
            else printf("No operand (implied)\n\n");
            break;
        default:
            printf("Bad CPU phase!\n\n");
    }
}

uint8_t CPU (nes con, operation *op, operation *inst)
{
    switch (con->CPU->PHS)
    {
        case FETCH:
            con->CPU->PHS = MEMOP;
            uint8_t cycle = IMM(con);
            *op = operation_table[*con->CPU->OPR];
            *inst = instruction_table[*con->CPU->OPR];
            return cycle;
        case MEMOP:
            con->CPU->PHS = WORK;
            if (*op)
                return (*op)(con);
            __attribute__((fallthrough));
        case WORK:
            con->CPU->PHS = FETCH;
            if (*inst)
                return (*inst)(con);
            return 0;
        default:
            con->CPU->PHS = FETCH;
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

void LOOP (nes con)
{
    uint8_t cpucycle = JMPB(con);
    printf("Initial jump to address: 0x%04X\n\n", con->CPU->PC);
    operation memptr;
    operation insptr;

    for (uint8_t clock = 0; 1; clock++)
    {
        if (clock == MAX_CLOCK)
            clock = 0;
        
        if ((clock % con->CPU->CLK) == 0)
        {
            if (cpucycle > 0)
                cpucycle--;
            else
            {
                cpucycle = CPU(con, &memptr, &insptr);
                DEBUG_CPU(con, &memptr);
            }
        }

        if ((clock % con->PPU->CLK) == 0)
        {
            // DO PPU CYCLE
        }
    }
}

void SET_TIMING (nes con, uint8_t timing)
{
    switch (timing)
    {
        case 1:
            con->CPU->CLK = CPU_PAL;
            con->PPU->CLK = PPU_PAL;
            break;
        default:
            con->CPU->CLK = CPU_NTSC;
            con->PPU->CLK = PPU_NTSC;
            break;
    }
}

int main (int argc, char *argv[])
{
    (void) argc;
    (void) argv;
    
    processor reg = {0};
    nesheader head = {0};
    cartridge crt = {0};
    graphics gpu = {0};
    audio aud = {0};
    console con = {&head, &reg, &gpu, &crt, &aud};
    //if (INIT_MEM(&mem)) return 1;
    FILE *rom = fopen("roms/test.nes", "rb");
    if (!rom) return 1;
    

    RESET_CPU(&con);
    if (LOAD_ROM(rom, &con))
        return 1;
    if (INIT_NT(&con))
        return 1;
    SET_TIMING(&con, con.HEAD->TIMING);

    LOOP(&con);

    FREE_NT(&con);
    //FREE_MEM(&mem);
    fclose(rom);
    return 0;
}
