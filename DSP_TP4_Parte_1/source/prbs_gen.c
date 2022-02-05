/*
 * prbs_gen.c
 *
 *  Created on: 6 ene. 2022
 *      Author: augus
 */
#include "prbs_gen.h"

uint16_t prbs(shift_register *pShift)
{
    char fb;
    fb = ((pShift -> bits).bit15 + (pShift -> bits).bit14 + (pShift -> bits).bit3 + (pShift -> bits).bit1)%2;
    pShift -> value = pShift->value << 1;
    (pShift -> bits).bit0 = fb;

    return (pShift -> value);
}

