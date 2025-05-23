//#include <time.h>
//#include <windows.h>
#include "nes2.h"

void LOOP (registers* reg, memory* mem, cartridge* cart)
{
    uint8_t* opr;
    uint8_t cycle;
    operation memptr;
    instruction insptr;
    //struct timespec t1, t2;
    while (1)
    {
        //timespec_get(&t1, TIME_UTC);
        
        opr = NULL;
        cycle = 0;
        
        cycle += IMM(reg, mem, cart, &opr);
        memptr = operation_table[*opr];
        insptr = instruction_table[*opr];
        if (memptr) cycle += memptr(reg, mem, cart, &opr);
        if (insptr) cycle += insptr(reg, opr);

        //timespec_get(&t2, TIME_UTC);
            
        //Sleep(roundl(((reg->C * cycle) - (t2.tv_nsec - t1.tv_nsec)) / 1000.0));
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

    FILE* rom = fopen("roms/test.nes", "rb");
    if (!rom) return 1;
    nesheader head = {0};
    cartridge cart = {0};
    if (LOAD_ROM(rom, &head, &mem, &cart))
        return 1;
    if (head.TIMING == 1) reg.C = PAL_CYCLE;
    else reg.C = NTSC_CYCLE;

    LOOP(&reg, &mem, &cart);

    FREE_MEM(&mem);
    fclose(rom);
    return 0;
}
