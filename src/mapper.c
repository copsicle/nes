#include "6502.h"
#include "mapper.h"
#include "memory.h"

uint8_t ALLOC_ROM (uint8_t*** bankptr, uint8_t** arrptr, memtype* map)
{
    *bankptr = (uint8_t**) calloc(map->COUNT, sizeof(uint8_t*));
    if (!*bankptr) return 1;
    for (uint8_t bank = 0; bank < map->COUNT; bank++)
    {
        (*bankptr)[bank] = (uint8_t*) calloc(map->SIZE, 1);
        if (!((*bankptr)[bank])) return 1;
    }
    if (arrptr)
    {
        uint8_t romcnt = (map->END - map->START) / map->SIZE;
        *arrptr = (uint8_t*) calloc(romcnt, 1);
        if (!(*arrptr)) return 1;
        for (uint8_t bank = 0; bank < romcnt; bank++)
            (*arrptr)[bank] = bank;
    }
    return 0;
}

void FREE_ROM (uint8_t*** bankptr, uint8_t** arrptr, memtype* map)
{
    if (*bankptr)
        for (uint8_t bank = 0; bank < map->COUNT; bank++)
            free((*bankptr)[bank]);
    if (arrptr) free(*arrptr);
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
    if (ALLOC_ROM(&(mem->PRG), &(mem->PRG_BANK), &(map->PRG)))
        return 1;
    if (map->CHR.COUNT)
        if (ALLOC_ROM(&(mem->CHR), &(mem->CHR_BANK), &(map->CHR)))
            return 1;
    if (map->RAM.COUNT)
        if (ALLOC_ROM(&(mem->RAM), NULL, &(map->RAM)))
            return 1;
    return 0;
}

void FREE_MAPS (maps* mem, mapper* map)
{
    FREE_ROM(&(mem->PRG), &(mem->PRG_BANK), &(map->PRG));
    FREE_ROM(&(mem->CHR), &(mem->CHR_BANK), &(map->CHR));
    FREE_ROM(&(mem->RAM), NULL, &(map->RAM));
}

uint8_t* CART_ADD (uint16_t add, memory* mem)
{
    (void) add;
    (void) mem;

    return NULL;
}
