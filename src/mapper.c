#include "types.h"
#include "mapper.h"

mapper mapper_table[0x1000] = {
    [0] = {{0x8000, 0xFFFF, 0x4000, 2}, {0x0000, 0x1FFF, 0x1000, 2}, {0}, {0}, {0}, {0}, 2},
    [1] = {{0x8000, 0xFFFF, 0x4000, 20}, {0x0000, 0x1FFF, 0x1000, 32}, {0x6000, 0x7FFF, 0x2000, 1}, {0}, {0}, {0}, 2},
};

uint8_t *TRANSLATE_MAP (uint16_t add, nes con)
{
    mapper *map = con->CART->MAP;
    if (add >= map->RAM.START && add <= map->RAM.END)
        return *(con->CART->RAM.PTR) + (add % map->RAM.SIZE);
    else if (add >= map->PRG.START && add <= map->PRG.END)
    {
        add -= map->PRG.START;
        return (con->CART->PRG.PTR[con->CART->PRG.ARR[add / map->PRG.SIZE]] + (add % map->PRG.SIZE));
    }
    return NULL;
}

uint64_t ROM_SIZE (uint16_t size)
{
    if (size < EXP_SIZE)
        return size * CHUNK_SIZE;
    else if (size < SHIFT_SIZE)
        return pow(2, ((size & 0xFC) >> 2)) * (((size & 0x03) * 2) + 1);
    else if (size == SHIFT_SIZE)
        return 0;
    return ((uint64_t) SHIFT_CHUNK) << (size & 0x0F);
}

uint8_t ALLOC_ROM (bank bnk, uint16_t size, memtype *map)
{
    // THIS FUNCTION IS MONOLITHIC AND SHOULD PROBABLY GET REPLACED
    uint64_t actual = ROM_SIZE(size);
    uint64_t sub = actual;
    //uint16_t mirror = (((uint64_t) map->SIZE) * map->COUNT) - actual;
    bnk->PTR = (uint8_t**) calloc(map->COUNT, sizeof(uint8_t*));
    uint8_t romcnt = (map->END - map->START + 1) / map->SIZE;
    //uint8_t mir = map->COUNT / 2;
    bnk->ARR = (uint8_t*) calloc(romcnt, 1);
    if (!(bnk->ARR) || !(bnk->PTR) || !actual)
        return 1;
    for (uint8_t bankcnt = 0; bankcnt < map->COUNT; bankcnt++)
    {
        if (sub > actual || sub == 0)
        {
            sub = 0;
            bnk->PTR[bankcnt] = bnk->PTR[(bankcnt % (actual / map->SIZE))];
            continue;
        }
        if (sub < map->SIZE)
            sub = 0;
        else
            sub -= map->SIZE;
        bnk->PTR[bankcnt] = (uint8_t*) calloc(map->SIZE, 1);
        if (!(bnk->PTR[bankcnt])) return 1;
        if (bankcnt < romcnt)
            bnk->ARR[bankcnt] = bankcnt;
    }
    return 0;
}

void FREE_ROM (bank bnk, memtype *map)
{
    if (bnk->PTR)
        for (uint8_t bankcnt = 0; bankcnt < map->COUNT; bankcnt++)
            free(bnk->PTR[bankcnt]);
    if (bnk->ARR) free(bnk->ARR);
}
/*
uint8_t ALLOC_PRG (maps *mem, mapper *map)
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

uint8_t ALLOC_CHR (maps *mem, mapper *map)
{

}

uint8_t ALLOC_RAM (maps *mem, mapper *map)
{

}
*/
uint8_t ALLOC_MAPS (nes con)
{
    if (ALLOC_ROM(&(con->CART->PRG), con->HEAD->PRGROM, &(con->CART->MAP->PRG)))
        return 1;
    if (con->CART->MAP->CHR.COUNT)
        if (ALLOC_ROM(&(con->CART->CHR), con->HEAD->CHRROM, &(con->CART->MAP->CHR)))
            return 1;
    if (con->CART->MAP->RAM.COUNT)
        if (ALLOC_ROM(&(con->CART->RAM), (con->HEAD->PRGSHIFT | SHIFT_SIZE), &(con->CART->MAP->RAM)))
            return 1;
    return 0;
}

void FREE_MAPS (cartridge *cart, mapper *map)
{
    FREE_ROM(&(cart->PRG), &(map->PRG));
    FREE_ROM(&(cart->CHR), &(map->CHR));
    FREE_ROM(&(cart->RAM), &(map->RAM));
}

uint8_t *CART_ADD (uint16_t add)
{
    (void) add;

    return NULL;
}
