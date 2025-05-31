#include "types.h"
#include "ppu.h"

uint8_t INIT_NT (nesheader* head, cartridge *cart)
{
    bank* mem = &(cart->NT);
    uint8_t ntlim = cart->MAP->NTCOUNT;
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
    switch (head->FLAGS & 0x09)
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

void FREE_NT (cartridge *cart)
{
    bank* mem = &(cart->NT);
    free(mem->ARR);
    for (uint8_t bankcnt = 0; bankcnt < cart->MAP->NTCOUNT; bankcnt++)
        free(mem->PTR[bankcnt]);
    free(mem->PTR);
}

