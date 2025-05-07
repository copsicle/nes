#include <stdint.h>
#include <stdlib.h>

#include "6502.h"
#include "memory.h"

int main(int argc, char *argv[])
{
    registers* regs;
    if ( !(regs = (registers*) calloc(1, sizeof(registers)))) return 1;
    memory* mem;
    if ( !(mem = (memory*) calloc(1, sizeof(memory)))) return 1;
    for (memmap* page = mem->ZEROPAGE; page < (mem + sizeof(memory)); page += sizeof(memmap))
    {
        *page 
    }

    uint8_t inst = 0x0A;
    regs->A = 0x80;
    uint8_t* opr = NULL;
    uint8_t cycles = 0;
    address_function_ptr ptr = addressing_modes[inst];
    if (ptr) cycles += ptr(regs, mem, opr);
    cycles += instructions[inst](regs, opr);
    
    free(mem->GENERAL);
    free(mem->STACK);
    free(mem->ZEROPAGE);
    free(mem);
    free(regs);
    return 0;
}