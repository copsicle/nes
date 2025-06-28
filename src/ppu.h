#ifndef _PPU_H
#define _PPU_H

#include <stdint.h>
#include <stdlib.h>

#define PT_PAGE 0x1000
#define NT_PAGE 0x0400
#define NT_COUNT 4
#define UNUSED_PAGE 0x0F00
#define PTRAM_SIZE 0x0020
#define PPU_SPACE 0x4000
#define OAM_SPACE 0x0100

#define LINE_SIZE 341

#define PPU_NTSC 4
#define H_NTSC 262

#define PPU_PAL 5
#define H_PAL 312

#define PPUCTRL 0x00
#define PPUMASK 0x01
#define PPUSTATUS 0x02
#define OAMADDR 0x03
#define OAMDATA 0x04
#define PPUSCROLL 0x05
#define PPUADDR 0x06
#define PPUDATA 0x07

#define V_X_LIMIT 0x001F
#define V_Y_LIMIT 0x03E0

struct nes;

uint8_t INIT_NT (nes con);
void FREE_NT (nes con);

#endif
