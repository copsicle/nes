#include "types.h"
#include "ppu.h"

uint8_t INIT_NT (nes con)
{
    bank mem = &(con->CART->NT);
    uint8_t ntlim = con->CART->MAP->NTCOUNT;
    mem->COUNT = NT_COUNT;
    mem->ARR = (uint8_t*) calloc(mem->COUNT, 1);
    mem->PTR = (uint8_t**) calloc(ntlim, sizeof(uint8_t*));
    if (!mem->ARR || !mem->PTR)
        return 1;
    for (uint8_t nt = 0; nt < ntlim; nt++)
    {
        mem->PTR[nt] = (uint8_t*) calloc(NT_PAGE, 1);
        if (!(mem->PTR[nt]))
            return 1;
    }
    switch (con->HEAD->FLAGS & 0x09)
    {
        case 0x00:
            mem->ARR[2] = 1;
            mem->ARR[3] = 1;
            break;
        case 0x01:
            mem->ARR[1] = 1;
            mem->ARR[3] = 1;
            break;
        default:
            if (ntlim == 4)
                for (uint8_t nt = 1; nt < ntlim; nt++)
                        mem->ARR[nt] = nt;
            // Other mappers here
            break;                    
    }
    return 0;
}

void FREE_NT (nes con)
{
    bank mem = &(con->CART->NT);
    free(mem->ARR);
    for (uint8_t bankcnt = 0; bankcnt < con->CART->MAP->NTCOUNT; bankcnt++)
        free(mem->PTR[bankcnt]);
    free(mem->PTR);
}

uint8_t BG_RENDER (nes con)
{
    if (con->PPU->SCL >= 240  && con->PPU->SCL <= 260)
    {
        if (con->PPU->SCL == 240 && con->PPU->CYC == 0)
            return 1; // BG lsbit
        else if (con->PPU->SCL == 241 && con->PPU->CYC == 1)
            return 1;
    }
    return 0;
}

void INC_V (nes con)
{
    uint16_t *cyc = &con->PPU->CYC;
    uint16_t *v = &con->PPU->V;

    if (*cyc <= 256 || *cyc > 320)
    {
        if ((*v & V_X_LIMIT) == V_X_LIMIT)
        {
            *v &= ~V_X_LIMIT;
            *v ^= NT_PAGE;
        }
        else (*v)++;
    }
    if (*cyc == 256)
    {
        if ((*v & 0x7000) != 0x7000)
            *v += 0x1000;
        else
        {
            *v &= ~0x7000;
            uint16_t y = (*v & V_Y_LIMIT) >> 5;
            switch (y)
            {
                case 29:
                    *v ^= 0x8000;
                    __attribute__((fallthrough));
                case 31:
                    y = 0;
                    break;
                default:
                    y++;
            }
            *v = (*v & ~V_Y_LIMIT) | (y << 5);
        }
    }
}

uint8_t VBLANK (nes con)
{
    uint16_t *cyc = &con->PPU->CYC;
    uint16_t *scl = &con->PPU->SCL;
    uint8_t *stat = &con->PPU->REG[PPUSTATUS];

    if (*scl == 240 && *cyc == 0)
        return 1; // BG lsbit
    if (*cyc == 1)
    {
        if (*scl == 241)
            *stat |= 0x80;
        else if (*scl == 261)
            *stat &= ~0xE0;
        return 1;
    }
    if (*scl != 261 || *cyc == 0)
        return 1;
    return 0;
}

uint8_t HBLANK (nes con)
{
    uint16_t *cyc = &con->PPU->CYC;
    uint16_t *scl = &con->PPU->SCL;
    uint16_t *v = &con->PPU->V;

    if (*scl == 261 && *cyc >= 280 && *cyc <= 304)
    {
        *v &= ~V_Y_LIMIT;
        *v |= con->PPU->T & V_Y_LIMIT;
    }

    switch (*cyc % 8)
    {
        case 5:
            __attribute__((fallthrough));
        case 6:
            break;
        case 7:
            __attribute__((fallthrough));
        case 0:
            break;
    }

    return 0;
}

uint8_t PPU_CYCLE (nes con)
{
    uint16_t *cyc = &con->PPU->CYC;
    uint16_t *scl = &con->PPU->SCL;
    uint16_t *v = &con->PPU->V;

    if (*scl > 239)
        if (VBLANK(con))
            return 0;

    if (*cyc >= 261 && *cyc <= 320)
        if (HBLANK(con))
            return 0;

    switch (*cyc % 8)
    {
        case 0:
            if (*cyc == 0)
                return 0; // BG lsbit
            INC_V(con);
            break;
        case 1:
            if (*cyc == 257)
            {
                *v &= ~V_X_LIMIT;
                *v |= con->PPU->T & V_X_LIMIT; 
                break;
            }
            __attribute__((fallthrough));
        case 2:
            break;
        case 3:
            __attribute__((fallthrough));
        case 4:
            break;
        case 5:
            __attribute__((fallthrough));
        case 6:
            break;
        case 7:
            break;
    }

    return 0;
}
