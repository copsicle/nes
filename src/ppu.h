#ifndef _PPU_H
#define _PPU_H

#include <stdint.h>

#define PATTERN_PAGE 0x1000
#define NAMETABLE_PAGE 0x0400
#define UNUSED_PAGE 0x0F00
#define PALLETE_SIZE 0x0020
#define PPU_SPACE 0x4000
#define OAM_SPACE 0x0100

#define LINE_SIZE 341

#define PPU_NTSC 4
#define H_NTSC 262

#define PPU_PAL 5
#define H_PAL 312

#endif
