#ifndef _MEMORY_H
#define _MEMORY_H

#define RAM_SIZE 0x0800
#define PPU_SIZE 0x08
#define APU_SIZE 0x14
#define IO_SIZE 0x04
#define TEST_SIZE 0x08
#define CART_SIZE 0xBFE0
#define SPACE_SIZE 0x10000

#define ZP_ADD 0x0000
#define STACK_ADD 0x0100

#include <stdint.h>
#include <stdio.h>

#include "types.h"
/*
uint8_t INIT_MEM(memory *mem);
void FREE_MEM(memory *mem);
*/
uint8_t READ_MEM_BYTE (uint16_t add, uint8_t off, nes con);
uint8_t JMPB (nes con);
uint8_t IMM (nes con);

extern const operation operation_table[0x100];

#endif
