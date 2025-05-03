#include <stdint.h>
#include <stdlib.h>

#include "6502.h"
#include "memory.h"

int main(int argc, char *argv[])
{
    struct registers* regs;
    if ( !(regs = (struct registers*) calloc(1, sizeof(struct registers)))) return 1;
    struct memory* mem;
    if ( !(mem = (struct memory*) calloc(1, sizeof(struct memory)))) return 1;
    mem->ZEROPAGE = (uint8_t*) calloc(0x100, 1);
    mem->STACK = (uint8_t*) calloc(0x100, 1);
    mem->GENERAL = (uint8_t*) calloc(0x600, 1);
    if  (
        ! mem->ZEROPAGE ||
        ! mem->STACK ||
        ! mem->GENERAL 
        )
        return 1;

    uint8_t inst = 0x0A;
    regs->A = 0x80;
    uint8_t* result = NULL;
    uint8_t cycles = 0;
    address_function_ptr ptr1 = addressing_modes[inst];
    if (ptr1) cycles += ptr1(regs, mem, result);
    cycles += instructions[inst](regs, result);
    
    free(mem->GENERAL);
    free(mem->STACK);
    free(mem->ZEROPAGE);
    free(mem);
    free(regs);
    return 0;
}