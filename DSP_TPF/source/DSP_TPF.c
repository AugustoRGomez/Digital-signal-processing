/*
 * TRABAJO FINAL DSP (Parte FRDM k64f)
 * Autores:
 * 	-Gomez, Augusto
 * 	-Alaniz, Dario
 * 	-Ferraris, Domingo
 *
 * PINES:
 *  FRDM-k64F:
 * 	 I2C:
 * 	  -SDA : J2[18]
 * 	  -SCLK : J2[20]
 * 	 FTM:
 * 	  -MCLK: J1[5]
 *
 * 	TWR-ELEV:
 * 	 I2C:
 * 	  -SDA : J8[8]
 * 	  -SCLK : J8[7]
 * 	 I2S:
 * 	  -MCLK : J8[21]
 */

#ifndef MAIN_INCLUDE
#define MAIN_INCLUDE

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"

#endif /* MAIN_INCLUDE */

/* TODO: insert other include files here. */
#include "sgtl5000.h"

/* TODO: insert other definitions and declarations here. */
uint16_t wBuff, rBuff;

/* TODO: Main */
int main(void) {
    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    sgtl5000_RW_test();

    while(1) {}
    return 0 ;
}
