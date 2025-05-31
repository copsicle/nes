#ifndef _MAPPER_H
#define _MAPPER_H

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define CHUNK_SIZE 0x4000
#define EXP_SIZE 0x0F00
#define SHIFT_SIZE 0xF000
#define SHIFT_CHUNK 0x40

struct cartridge;
struct nesheader;
struct mapper;

uint8_t ALLOC_MAPS (cartridge *cart, nesheader* head);
uint8_t *TRANSLATE_MAP (uint16_t add, cartridge *cart);

extern mapper mapper_table[0x1000];

#endif
