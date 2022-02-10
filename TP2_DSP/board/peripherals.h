/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_pit.h"
#include "fsl_uart.h"
#include "fsl_clock.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
/* Definitions for BOARD_InitPeripherals functional group */
/* Alias for GPIOA peripheral */
#define GPIOA_GPIO GPIOA
/* Alias for PORTA */
#define GPIOA_PORT PORTA
/* GPIOA interrupt vector ID (number). */
#define GPIOA_IRQN PORTA_IRQn
/* GPIOA interrupt handler identifier. */
#define GPIOA_IRQHANDLER PORTA_IRQHandler
/* Alias for GPIOC peripheral */
#define GPIOC_GPIO GPIOC
/* Alias for PORTC */
#define GPIOC_PORT PORTC
/* GPIOC interrupt vector ID (number). */
#define GPIOC_IRQN PORTC_IRQn
/* GPIOC interrupt handler identifier. */
#define GPIOC_IRQHANDLER PORTC_IRQHandler
/* BOARD_InitPeripherals defines for PIT */
/* Definition of peripheral ID. */
#define PIT_PERIPHERAL PIT
/* Definition of clock source. */
#define PIT_CLOCK_SOURCE kCLOCK_BusClk
/* Definition of clock source frequency. */
#define PIT_CLK_FREQ CLOCK_GetFreq(PIT_CLOCK_SOURCE)
/* Definition of ticks count for channel 1 - deprecated. */
#define PIT_1_TICKS USEC_TO_COUNT(1000000U, PIT_CLK_FREQ) - 1U
/* Definition of channel number for channel 1. */
#define PIT_CHANNEL_1 kPIT_Chnl_1
/* Definition of ticks count for channel 1. */
#define PIT_CHANNEL_1_TICKS USEC_TO_COUNT(1000000U, PIT_CLK_FREQ) - 1U
/* Definition of peripheral ID */
#define UART0_PERIPHERAL UART0
/* Definition of the clock source frequency */
#define UART0_CLOCK_SOURCE CLOCK_GetFreq(UART0_CLK_SRC)
/* UART0 interrupt vector ID (number). */
#define UART0_SERIAL_RX_TX_IRQN UART0_RX_TX_IRQn
/* UART0 interrupt handler identifier. */
#define UART0_SERIAL_RX_TX_IRQHANDLER UART0_RX_TX_IRQHandler
/* UART0 interrupt vector ID (number). */
#define UART0_SERIAL_ERROR_IRQN UART0_ERR_IRQn
/* UART0 interrupt handler identifier. */
#define UART0_SERIAL_ERROR_IRQHANDLER UART0_ERR_IRQHandler

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
extern const pit_config_t PIT_config;
extern const uart_config_t UART0_config;

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void);

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void);

#if defined(__cplusplus)
}
#endif

#endif /* _PERIPHERALS_H_ */