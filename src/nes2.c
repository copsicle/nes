#include "nes2.h"

uint8_t NES2_HEADER (FILE* rom, nesheader* head)
{
    uint8_t read = 0;
    uint8_t res = fread(&read, 1, 1, rom);
    head->MAPPER |= ((uint16_t)read) << 8;
    res += fread(&read, 1, 1, rom);
    head->PRGROM |= ((uint16_t)(read & 0x0F)) << 8;
    head->CHRROM |= ((uint16_t)(read & 0xF0)) << 4;
    res += fread(&(head->PRGSHIFT), 1, 1, rom);
    res += fread(&(head->CHRSHIFT), 1, 1, rom);
    res += fread(&read, 1, 1, rom);
    head->TIMING = read & 0x03;
    res += fread(&read, 1, 1, rom);
    if ((head->FLAGS & 0x30) == 0x30)
        head->SYSTYPE = read & 0x0F;
    else head->SYSTYPE = read;
    res += fread(&read, 1, 1, rom);
    head->MISCROMS = read & 0x03;
    res += fread(&read, 1, 1, rom);
    head->DEFEXP = read & 0x3F;
    return (res != 8);
}

uint8_t INES_HEADER (FILE* rom, nesheader* head)
{
    uint8_t res = fread(&head->PRGSHIFT, 1, 1, rom);
    uint8_t read = 0;
    res += fread(&read, 1, 1, rom);
    head->TIMING = read & 0x01;
    res += fread(&read, 1, 1, rom);
    head->TIMING |= ((read & 0x33) << 2);
    int seek = fseek(rom, 5, SEEK_CUR);
    return (seek != 0 || res != 3);
}

uint8_t READ_HEADER (FILE* rom, nesheader* head)
{
    uint8_t res = fread(head->ID, sizeof(ID_STR) - 1, 1, rom);
    res += fread(&head->PRGROM, 1, 1, rom);
    res += fread(&head->CHRROM, 1, 1, rom);
    uint8_t read = 0;
    res += fread(&read, 1, 1, rom);
    head->FLAGS = read & 0x0F;
    head->MAPPER = read >> 4;
    res += fread(&read, 1, 1, rom);
    head->FLAGS |= read << 4;
    head->MAPPER |= read & 0xF0;
    if (res != (sizeof(ID_STR) + 4) || strcmp(head->ID, ID_STR))
        return 1;
    if ((head->FLAGS & 0xC0) == 0x80)
        return NES2_HEADER(rom, head);
    return INES_HEADER(rom, head);
}

uint8_t LOAD_TRAINER (FILE* rom, memory* mem)
{
    uint8_t* ptr = NULL;
    READ_MEM_BYTE(TRAINER_ADD, 0, mem, ptr);
    return (fread(ptr, 1, TRAINER_SIZE, rom) != TRAINER_SIZE);
}

uint8_t LOAD_DATA (FILE* rom, uint8_t*** mem, uint16_t banksize, uint8_t bankcnt)
{
    for (uint8_t bank = 0; bank < bankcnt; bank++)
        if (fread((*mem)[bank], 1, banksize, rom) != banksize)
            return 1;
    return 0;
}
/*
uint32_t CALC_ROM (uint16_t area, uint16_t unit)
{
    uint32_t size = 8192;
    uint8_t conv = (uint8_t) area;
    if ((area & 0x0F00) == 0x0F00)
        size = pow(2, conv >> 2) * (((conv & 0x03) * 2) + 1);
    else if (area)
        size = unit * area;
    return size;
}

uint8_t LOAD_PRG (FILE* rom, nesheader* head, ppumem* ppu)
{
    uint32_t size = CALC_ROM(head->PRGROM, PRG_UNITS);
    fread(void *Buffer, 1, size, rom);
    // ADD ALLOCATION AND INITIAL MAPPING
    return 0;
}

uint8_t LOAD_CHR (FILE* rom, nesheader* head, ppumem* ppu)
{
    uint32_t size = CALC_ROM(head->CHRROM, CHR_UNITS);
    fread(void *Buffer, 1, size, rom);
    // ADD ALLOCATION AND INITIAL MAPPING
    return 0;
}

uint8_t LOAD_MISC (FILE* rom)
{
    long loc = ftell(rom);
    int size = fseek(rom, 0, SEEK_END);
    if (fseek(rom, loc, SEEK_SET) != loc) return 1;
    fread(void *Buffer, 1, size, rom);
    // ADD ALLOCATION AND INITIAL MAPPING
    return 0;
}
*/
uint8_t LOAD_ROM (FILE* rom, nesheader* head, memory* mem, mapper* map)
{
    if (READ_HEADER(rom, head)) return 1;
    if (head->FLAGS & 0x04)
        if (LOAD_TRAINER(rom, mem))
            return 1;
    
    // TODO: CHOOSE MAPPER HERE

    if (ALLOC_MAPS(mem->CART, map))
        return 1;
    if (LOAD_DATA(rom, &(mem->CART->PRG), map->PRG_END - map->PRG_START, map->PRG_MAP_COUNT))
        return 1;
    if (LOAD_DATA(rom, &(mem->CART->CHR), map->CHR_END - map->CHR_START, map->CHR_MAP_COUNT))
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