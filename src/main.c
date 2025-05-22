#include <stdio.h>
#include <direct.h>
#include "6502.h"
#include "mapper.h"
#include "memory.h"
#include "nes2.h"

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    registers* regs = NULL;
    if (! (regs = (registers*) calloc(1, sizeof(registers))))
        return 1;
    memory* mem = NULL;
    if (! (mem = (memory*) calloc(1, sizeof(memory))))
        return 1;
    if (INIT_MEM(mem)) return 1;


    printf("%s", _getcwd(NULL, 0));
    FILE* rom = fopen("src/test/test.nes", "rb");
    if (!rom) return 1;
    nesheader* head = NULL;
    if (! (head = (nesheader*) calloc(1, sizeof(nesheader))))
        return 1;
    if (LOAD_ROM(rom, head, mem)) return 1;

    uint8_t inst = 0x6A;
    regs->A = 0x80;
    uint8_t* opr = NULL;
    uint8_t cycles = 0;
    address_function_ptr ptr = addressing_modes[inst];
    if (ptr) cycles += ptr(regs, mem, &opr);
    cycles += instructions[inst](regs, opr);
    
    printf("%d %d", regs->A, cycles);

    FREE_MEM(mem);
    free(mem);
    free(regs);
    return 0;
}
