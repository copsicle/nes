#include "types.h"
#include "memory.h"
#include "mapper.h"
#include "nes2.h"

uint8_t NES2_HEADER (FILE *rom, nes con)
{
    uint8_t read = 0;
    uint8_t res = fread(&read, 1, 1, rom);
    con->HEAD->MAPPER |= ((uint16_t)read) << 8;
    res += fread(&read, 1, 1, rom);
    con->HEAD->PRGROM |= ((uint16_t)(read & 0x0F)) << 8;
    con->HEAD->CHRROM |= ((uint16_t)(read & 0xF0)) << 4;
    res += fread(&(con->HEAD->PRGSHIFT), 1, 1, rom);
    res += fread(&(con->HEAD->CHRSHIFT), 1, 1, rom);
    res += fread(&read, 1, 1, rom);
    con->HEAD->TIMING = read & 0x03;
    res += fread(&read, 1, 1, rom);
    if ((con->HEAD->FLAGS & 0x30) == 0x30)
        con->HEAD->SYSTYPE = read & 0x0F;
    else con->HEAD->SYSTYPE = read;
    res += fread(&read, 1, 1, rom);
    con->HEAD->MISCROMS = read & 0x03;
    res += fread(&read, 1, 1, rom);
    con->HEAD->DEFEXP = read & 0x3F;
    return (res != 8);
}

uint8_t INES_HEADER (FILE *rom, nes con)
{
    uint8_t res = fread(&con->HEAD->PRGSHIFT, 1, 1, rom);
    uint8_t read = 0;
    res += fread(&read, 1, 1, rom);
    con->HEAD->TIMING = read & 0x01;
    res += fread(&read, 1, 1, rom);
    con->HEAD->TIMING |= ((read & 0x33) << 2);
    int seek = fseek(rom, 5, SEEK_CUR);
    return (seek != 0 || res != 3);
}

uint8_t READ_HEADER (FILE *rom, nes con)
{
    uint8_t res = fread(con->HEAD->ID, 1, sizeof(ID_STR) - 1, rom);
    res += fread(&con->HEAD->PRGROM, 1, 1, rom);
    res += fread(&con->HEAD->CHRROM, 1, 1, rom);
    uint8_t read = 0;
    res += fread(&read, 1, 1, rom);
    con->HEAD->FLAGS = read & 0x0F;
    con->HEAD->MAPPER = read >> 4;
    res += fread(&read, 1, 1, rom);
    con->HEAD->FLAGS |= read << 4;
    con->HEAD->MAPPER |= read & 0xF0;
    if (res != (sizeof(ID_STR) + 3) || strcmp(con->HEAD->ID, ID_STR))
        return 1;
    if ((con->HEAD->FLAGS & 0xC0) == 0x80)
        return NES2_HEADER(rom, con);
    return INES_HEADER(rom, con);
}

uint8_t LOAD_TRAINER (FILE *rom, nes con)
{
    READ_MEM_BYTE(TRAINER_ADD, 0, con);
    return (fread(con->CPU->OPR, 1, TRAINER_SIZE, rom) != TRAINER_SIZE);
}

uint8_t LOAD_DATA (FILE *rom, bank bnk, memtype *map)
{
    for (uint8_t bankcnt = 0; bankcnt < map->COUNT; bankcnt++)
        if (fread(bnk->PTR[bankcnt], 1, map->SIZE, rom) != map->SIZE)
            return 1;
    return 0;
}
/*
uint32_t CALC_ROM (uint16_t area, uint16_t unit)
{
    uint32_t size = 8192;
    uint8_t conv = (uint8_t) area;
    if ((area & 0x0F00) == 0x0F00)
        size = pow(2, conv >> 2) * (((conv & 0x03)  *2) + 1);
    else if (area)
        size = unit * area;
    return size;
}

uint8_t LOAD_PRG (FILE *rom, nesheader *head, ppumem *ppu)
{
    uint32_t size = CALC_ROM(head->PRGROM, PRG_UNITS);
    fread(void *Buffer, 1, size, rom);
    // ADD ALLOCATION AND INITIAL MAPPING
    return 0;
}

uint8_t LOAD_CHR (FILE *rom, nesheader *head, ppumem *ppu)
{
    uint32_t size = CALC_ROM(head->CHRROM, CHR_UNITS);
    fread(void *Buffer, 1, size, rom);
    // ADD ALLOCATION AND INITIAL MAPPING
    return 0;
}

uint8_t LOAD_MISC (FILE *rom)
{
    long loc = ftell(rom);
    int size = fseek(rom, 0, SEEK_END);
    if (fseek(rom, loc, SEEK_SET) != loc) return 1;
    fread(void *Buffer, 1, size, rom);
    // ADD ALLOCATION AND INITIAL MAPPING
    return 0;
}
*/
uint8_t LOAD_ROM (FILE *rom, nes con)
{
    if (READ_HEADER(rom, con)) return 1;
    if (con->HEAD->FLAGS & 0x04)
        if (LOAD_TRAINER(rom, con))
            return 1;
    
    con->CART->MAP = &(mapper_table[con->HEAD->MAPPER & 0x0FFF]);

    if (ALLOC_MAPS(con))
        return 1;
    if (LOAD_DATA(rom, &(con->CART->PRG), &(con->CART->MAP->PRG)))
        return 1;
    if (LOAD_DATA(rom, &(con->CART->CHR), &(con->CART->MAP->CHR)))
        return 1;
    
    /*
    if (LOAD_PRG(rom, head, ppu)) return 1;
    if (LOAD_CHR(rom, head, ppu)) return 1;
    
    if (!(feof(rom)))
        if (LOAD_MISC(rom))
            return 1;
    */
    return 0;
}
