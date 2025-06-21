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

uint8_t PPU_CYCLE (nes con)
{
    switch (con->PPU->CYC % 8)
    {
        case 0:
            switch (con->PPU->SCL)
            {
                case 0:
                    if (con->PPU->EVN)
                        return 1; // skip frame
                    else if (!(con->PPU->CYC))
                        return 1;  // BG lsbit
            }
            if (!(con->PPU->SCL))
            {
                if (con->PPU->EVN)
                    return 1; // skip even frame
                
            }
            return 1; // do BG lsbit
            break;
        case 1:
            //con->PPU->V;
        case 2:
        // NT high(?) byte fetch
        case 3:
        // AT 
        default:
            return 0;
    }
}
