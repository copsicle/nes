#include "mapper.h"

uint8_t ALLOC_ROM (uint8_t*** bankptr, uint8_t** arrptr, uint8_t mapcnt, uint16_t banksize, uint16_t memsize)
{
    *bankptr = (uint8_t**) calloc(mapcnt, sizeof(uint8_t*));
    if (!*bankptr) return 1;
    for (uint8_t bank = 0; bank < mapcnt; bank++)
    {
        (*bankptr)[bank] = (uint8_t*) calloc(banksize, 1);
        if (!((*bankptr)[bank])) return 1;
    }
    if (arrptr)
    {
        uint16_t romcnt = memsize / banksize;
        *arrptr = (uint8_t*) calloc(romcnt, 1);
        if (!(*arrptr)) return 1;
        for (uint8_t bank = 0; bank < romcnt; bank++)
            (*arrptr)[bank] = bank;
    }
    return 0;
}
/*
uint8_t ALLOC_PRG (maps* mem, mapper* map)
{
    mem->PRG = (uint8_t**) calloc(map->PRG_MAP_COUNT, sizeof(uint8_t*));
    if (!mem->PRG) return 1;
    for (uint8_t bank = 0; bank < map->PRG_MAP_COUNT; bank++)
    {
        mem->PRG[bank] = (uint8_t*) calloc(map->PRG_BANK_SIZE, 1);
        if (!mem->PRG[bank]) return 1;
    }
    uint16_t prgsize = (map->PRG_END - map->PRG_START) / map->PRG_BANK_SIZE;
    mem->PRG_BANK = (uint8_t*) calloc(prgsize, 1);
    if (!mem->PRG_BANK) return 1;
    for (uint8_t bank = 0; bank < prgsize; bank++)
        mem->PRG_BANK[bank] = bank;
    return 0;
}

uint8_t ALLOC_CHR (maps* mem, mapper* map)
{

}

uint8_t ALLOC_RAM (maps* mem, mapper* map)
{

}
*/
uint8_t ALLOC_MAPS (maps* mem, mapper* map)
{
    if (ALLOC_ROM(&(mem->PRG), &(mem->PRG_BANK), map->PRG_MAP_COUNT, map->PRG_BANK_SIZE, map->PRG_END - map->PRG_START))
        return 1;
    if (map->CHR_BANK_SIZE)
        if (ALLOC_ROM(&(mem->CHR), &(mem->CHR_BANK), map->CHR_MAP_COUNT, map->CHR_BANK_SIZE, map->CHR_END - map->CHR_START))
            return 1;
    if (map->PRG_RAM_EN)
        if (ALLOC_ROM(&(mem->RAM), NULL, 1, 1, map->PRG_RAM_END - map->PRG_RAM_START))
            return 1;
    return 0;
}

