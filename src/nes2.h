#ifndef _NES2_H
#define _NES2_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define ID_STR "NES\x1A"
#define TRAINER_ADD 0x7000
#define TRAINER_SIZE 0x0200
#define PRG_UNITS 0x4000
#define CHR_UNITS 0x2000

struct nesheader;
struct memory;
struct cartridge;

uint8_t LOAD_ROM (FILE *rom, nesheader *head, memory *mem, cartridge *cart);

#endif
