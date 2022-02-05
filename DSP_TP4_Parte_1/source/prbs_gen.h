/*
 * prbs_gen.h
 *
 *  Created on: 6 ene. 2022
 *      Author: augus
 */

#ifndef PRBS_GEN_H_
#define PRBS_GEN_H_

#include "arm_math.h"

typedef union
{
	uint16_t value;
    struct
    {
        uint8_t bit0 : 1;
        uint8_t bit1 : 1;
        uint8_t bit2 : 1;
        uint8_t bit3 : 1;
        uint8_t bit4 : 1;
        uint8_t bit5 : 1;
        uint8_t bit6 : 1;
        uint8_t bit7 : 1;
        uint8_t bit8 : 1;
        uint8_t bit9 : 1;
        uint8_t bit10 : 1;
        uint8_t bit11 : 1;
        uint8_t bit12 : 1;
        uint8_t bit13 : 1;
        uint8_t bit14 : 1;
        uint8_t bit15 : 1;
    } bits;
} shift_register;

uint16_t prbs(shift_register *pShift);

#endif /* PRBS_GEN_H_ */
