#include "memory.h"

const mapper mapper_table[0x1000] = {
    [0] = {{0x8000, 0xFFFF, 0x8000, 1}, {0x0000, 0x1FFF, 0x2000, 1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
    [1] = {{0x8000, 0xFFFF, 0x4000, 20}, {0x0000, 0x1FFF, 0x1000, 32}, {0x6000, 0x7FFF, 0x2000, 1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
};

uint8_t* SEARCH_BANKS (uint8_t** banks ,uint8_t* arr, uint16_t size, uint16_t offadd)
{
    return (banks[arr[offadd / size]] + (offadd % size));
}

uint8_t* TRANSLATE_MAP (uint16_t add, cartridge* cart)
{
    if (add >= cart->MAP->RAM.START && add <= cart->MAP->RAM.END)
        return *(cart->RAM) + (add % cart->MAP->RAM.SIZE);
    else if (add >= cart->MAP->PRG.START && add <= cart->MAP->PRG.END)
        return SEARCH_BANKS(cart->PRG, cart->PRG_BANK, cart->MAP->PRG.SIZE, add - cart->MAP->PRG.START);
    else if (add >= cart->MAP->CHR.START && add <= cart->MAP->CHR.END)
        return SEARCH_BANKS(cart->CHR, cart->CHR_BANK, cart->MAP->CHR.SIZE, add - cart->MAP->CHR.START);
    return NULL;
}

uint8_t ALLOC_ROM (uint8_t*** bankptr, uint8_t** arrptr, const memtype* map)
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
        uint8_t romcnt = (map->END - map->START + 1) / map->SIZE;
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
uint8_t ALLOC_MAPS (cartridge* cart)
{
    if (ALLOC_ROM(&(cart->PRG), &(cart->PRG_BANK), &(cart->MAP->PRG)))
        return 1;
    if (cart->MAP->CHR.COUNT)
        if (ALLOC_ROM(&(cart->CHR), &(cart->CHR_BANK), &(cart->MAP->CHR)))
            return 1;
    if (cart->MAP->RAM.COUNT)
        if (ALLOC_ROM(&(cart->RAM), NULL, &(cart->MAP->RAM)))
            return 1;
    return 0;
}

void FREE_MAPS (cartridge* mem, mapper* map)
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
