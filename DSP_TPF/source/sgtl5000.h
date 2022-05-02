/*
 * sgtl5000.h
 *
 *  Created on: 8 mar. 2022
 *      Author: augus
 */

#ifndef SGTL5000_H_
#define SGTL5000_H_

#define SGTL5000_CHIP_ID                0x0000
#define SGTL5000_CHIP_DIG_POWER         0x0002
#define SGTL5000_CHIP_CLK_CTRL          0x0004
#define SGTL5000_CHIP_I2S_CTRL          0x0006
#define SGTL5000_CHIP_SSS_CTRL          0x000a
#define SGTL5000_CHIP_ADCDAC_CTRL       0x000e
#define SGTL5000_CHIP_DAC_VOL           0x0010
#define SGTL5000_CHIP_PAD_STRENGTH      0x0014
#define SGTL5000_CHIP_ANA_ADC_CTRL      0x0020
#define SGTL5000_CHIP_ANA_HP_CTRL       0x0022
#define SGTL5000_CHIP_ANA_CTRL          0x0024
#define SGTL5000_CHIP_LINREG_CTRL       0x0026
#define SGTL5000_CHIP_REF_CTRL          0x0028
#define SGTL5000_CHIP_MIC_CTRL          0x002a
#define SGTL5000_CHIP_LINE_OUT_CTRL     0x002c
#define SGTL5000_CHIP_LINE_OUT_VOL      0x002e
#define SGTL5000_CHIP_ANA_POWER         0x0030
#define SGTL5000_CHIP_PLL_CTRL          0x0032
#define SGTL5000_CHIP_CLK_TOP_CTRL      0x0034
#define SGTL5000_CHIP_ANA_STATUS        0x0036
#define SGTL5000_CHIP_ANA_TEST1         0x0038
#define SGTL5000_CHIP_SHORT_CTRL        0x003c
#define SGTL5000_CHIP_ANA_TEST2         0x003a
#define SGTL5000_CHIP_DAP_BASS_ENHANCE          0x0104
#define SGTL5000_CHIP_DAP_BASS_ENHANCE_CTRL     0x0106

#define SGTL5000_DAP_AUDIO_EQ                   0x0108
#define SGTL5000_DAP_AUDIO_EQ_BASS_BAND0        0x0116
#define SGTL5000_DAP_AUDIO_EQ_BASS_BAND1        0x0118
#define SGTL5000_DAP_AUDIO_EQ_BASS_BAND2        0x011A
#define SGTL5000_DAP_AUDIO_EQ_BASS_BAND3        0x011C
#define SGTL5000_DAP_AUDIO_EQ_BASS_BAND4        0x011E

#define SGTL5000_CHIP_DAP_AVC_CTRL              0x0124
#define SGTL5000_CHIP_DAP_MAIN_CHAN             0x0120
#define SGTL5000_CHIP_DAP_CTRL                  0x0100

#define SGTL5000_DAP_AVC_THRESHOLD				0x0126
#define SGTL5000_DAP_AVC_ATTACK					0x0128
#define SGTL5000_DAP_AVC_DECAY					0x012A




#ifndef MAIN_INCLUDE
#define MAIN_INCLUDE

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"

#endif

#define I2C_SLAVE_ADDR 0x0A
#define WAIT_TIME 100U

uint16_t sgtl5000_read_register(uint16_t addr);
void sgtl5000_write_register(uint16_t wBuff, uint16_t addr);
void sgtl5000_modify_register(uint16_t addr, uint16_t newBits, uint8_t bitLen, uint8_t bitShift);
void sgtl5000_RW_test();
void sgtl5000_power_up();
void sgtl5000_IO_routing();
void sgtl5000_volume_setup();
void sgtl5000_clk_setup();
void sgtl5000_dap_setup();
void sgtl5000_i2s_setup();
void sgtl5000_init_Line_in_HP_out_32K();
void sgtl5000_init_Line_in_AVC_HP_out_32K();
void sgtl5000_init_MIC_in_AVC_HP_out_32K();
void sgtl5000_init_Line_in_AVC_Line_out_32K();

#endif /* SGTL5000_H_ */
