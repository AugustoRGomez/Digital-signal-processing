/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "fsl_edma.h"
#include "fsl_dmamux.h"
#include "fsl_common.h"
#include "fsl_i2c.h"
#include "fsl_sai.h"
#include "fsl_sai_edma.h"
#include "fsl_clock.h"
#include "fsl_uart.h"
#include "fsl_adc16.h"
#include "fsl_pit.h"
#include "fsl_gpio.h"
#include "fsl_port.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
/* Definitions for BOARD_InitPeripherals functional group */
/* Used DMA device. */
#define DMA_DMA_BASEADDR DMA0
/* Associated DMAMUX device that is used for muxing of requests. */
#define DMA_DMAMUX_BASEADDR DMAMUX
/* BOARD_InitPeripherals defines for I2C0 */
/* Definition of peripheral ID */
#define I2C0_PERIPHERAL I2C0
/* Definition of the clock source */
#define I2C0_CLOCK_SOURCE I2C0_CLK_SRC
/* Definition of the clock source frequency */
#define I2C0_CLK_FREQ CLOCK_GetFreq(I2C0_CLOCK_SOURCE)
/* Definition of peripheral ID */
#define I2S0_PERIPHERAL I2S0
/* Master clock source frequency used for calculating the master clock divider, not available on all devices. */
#define I2S0_MCLK_SOURCE_CLOCK_HZ 120000000UL
/* Master clock value set by the user to the Master clock frequency item. */
#define I2S0_USER_MCLK_HZ 8192000UL
/* Bit clock source frequency used for calculating the bit clock divider in the TxSetBitClockRate function. */
#define I2S0_TX_BCLK_SOURCE_CLOCK_HZ 8192000UL
/* Bit clock source frequency used for calculating the bit clock divider in the RxSetBitClockRate function. */
#define I2S0_RX_BCLK_SOURCE_CLOCK_HZ 8192000UL
/* Sample rate used for calculating the bit clock divider in the TxSetBitClockRate function. */
#define I2S0_TX_SAMPLE_RATE 32000UL
/* Sample rate used for calculating the bit clock divider in the RxSetBitClockRate function. */
#define I2S0_RX_SAMPLE_RATE 32000UL
/* Word width used for calculating the bit clock divider in the TxSetBitClockRate function. */
#define I2S0_TX_WORD_WIDTH 32U
/* Word width used for calculating the bit clock divider in the RxSetBitClockRate function. */
#define I2S0_RX_WORD_WIDTH 32U
/* Number of words within frame used for calculating the bit clock divider in the TxSetBitClockRate function. */
#define I2S0_TX_WORDS_PER_FRAME 2U
/* Number of words within frame used for calculating the bit clock divider in the RxSetBitClockRate function. */
#define I2S0_RX_WORDS_PER_FRAME 2U
/* I2S0 eDMA source request. */
#define I2S0_TX_DMA_REQUEST kDmaRequestMux0I2S0Tx
/* Selected eDMA channel number. */
#define I2S0_TX_DMA_CHANNEL 1
/* DMAMUX device that is used for muxing of the request. */
#define I2S0_TX_DMAMUX_BASEADDR DMAMUX
/* Used DMA device. */
#define I2S0_TX_DMA_BASEADDR DMA0
/* I2S0 eDMA source request. */
#define I2S0_RX_DMA_REQUEST kDmaRequestMux0I2S0Rx
/* Selected eDMA channel number. */
#define I2S0_RX_DMA_CHANNEL 0
/* DMAMUX device that is used for muxing of the request. */
#define I2S0_RX_DMAMUX_BASEADDR DMAMUX
/* Used DMA device. */
#define I2S0_RX_DMA_BASEADDR DMA0
/* Definition of peripheral ID */
#define UART0_PERIPHERAL UART0
/* Definition of the clock source frequency */
#define UART0_CLOCK_SOURCE CLOCK_GetFreq(UART0_CLK_SRC)
/* Alias for ADC0 peripheral */
#define ADC0_PERIPHERAL ADC0
/* ADC0 interrupt vector ID (number). */
#define ADC0_IRQN ADC0_IRQn
/* ADC0 interrupt handler identifier. */
#define ADC0_IRQHANDLER ADC0_IRQHandler
/* Channel 0 (SE.0) conversion control group. */
#define ADC0_CH0_CONTROL_GROUP 0
/* Channel 1 (SE.1) conversion control group. */
#define ADC0_CH1_CONTROL_GROUP 0
/* BOARD_InitPeripherals defines for PIT */
/* Definition of peripheral ID. */
#define PIT_PERIPHERAL PIT
/* Definition of clock source. */
#define PIT_CLOCK_SOURCE kCLOCK_BusClk
/* Definition of clock source frequency. */
#define PIT_CLK_FREQ CLOCK_GetFreq(PIT_CLOCK_SOURCE)
/* Definition of ticks count for channel 0 - deprecated. */
#define PIT_0_TICKS USEC_TO_COUNT(5000U, PIT_CLK_FREQ) - 1U
/* Definition of ticks count for channel 1 - deprecated. */
#define PIT_1_TICKS USEC_TO_COUNT(250U, PIT_CLK_FREQ) - 1U
/* PIT interrupt vector ID (number) - deprecated. */
#define PIT_0_IRQN PIT0_IRQn
/* PIT interrupt handler identifier - deprecated. */
#define PIT_0_IRQHANDLER PIT0_IRQHandler
/* PIT interrupt vector ID (number) - deprecated. */
#define PIT_1_IRQN PIT1_IRQn
/* PIT interrupt handler identifier - deprecated. */
#define PIT_1_IRQHANDLER PIT1_IRQHandler
/* Definition of channel number for channel 0. */
#define PIT_CHANNEL_0 kPIT_Chnl_0
/* Definition of channel number for channel 1. */
#define PIT_CHANNEL_1 kPIT_Chnl_1
/* Definition of ticks count for channel 0. */
#define PIT_CHANNEL_0_TICKS USEC_TO_COUNT(5000U, PIT_CLK_FREQ) - 1U
/* Definition of ticks count for channel 1. */
#define PIT_CHANNEL_1_TICKS USEC_TO_COUNT(250U, PIT_CLK_FREQ) - 1U
/* PIT interrupt vector ID (number). */
#define PIT_CHANNEL_0_IRQN PIT0_IRQn
/* PIT interrupt handler identifier. */
#define PIT_CHANNEL_0_IRQHANDLER PIT0_IRQHandler
/* PIT interrupt vector ID (number). */
#define PIT_CHANNEL_1_IRQN PIT1_IRQn
/* PIT interrupt handler identifier. */
#define PIT_CHANNEL_1_IRQHANDLER PIT1_IRQHandler
/* Alias for GPIOC peripheral */
#define GPIOC_GPIO GPIOC
/* Alias for PORTC */
#define GPIOC_PORT PORTC
/* GPIOC interrupt vector ID (number). */
#define GPIOC_IRQN PORTC_IRQn
/* GPIOC interrupt handler identifier. */
#define GPIOC_IRQHANDLER PORTC_IRQHandler
/* Alias for GPIOA peripheral */
#define GPIOA_GPIO GPIOA
/* Alias for PORTA */
#define GPIOA_PORT PORTA
/* GPIOA interrupt vector ID (number). */
#define GPIOA_IRQN PORTA_IRQn
/* GPIOA interrupt handler identifier. */
#define GPIOA_IRQHANDLER PORTA_IRQHandler

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
extern const edma_config_t DMA_config;
extern const i2c_master_config_t I2C0_config;
extern sai_transceiver_t I2S0_Tx_config;
extern sai_transceiver_t I2S0_Rx_config;
extern edma_handle_t I2S0_TX_Handle;
extern edma_handle_t I2S0_RX_Handle;
extern sai_edma_handle_t I2S0_SAI_Tx_eDMA_Handle;
extern sai_edma_handle_t I2S0_SAI_Rx_eDMA_Handle;
extern const uart_config_t UART0_config;
extern adc16_channel_config_t ADC0_channelsConfig[2];
extern const adc16_config_t ADC0_config;
extern const adc16_channel_mux_mode_t ADC0_muxMode;
extern const pit_config_t PIT_config;

/***********************************************************************************************************************
 * Callback functions
 **********************************************************************************************************************/
/* SAI transfer Tx callback function for the I2S0 component (init. function BOARD_InitPeripherals)*/
extern void I2S_TX_eDMA_callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
/* SAI transfer Rx callback function for the I2S0 component (init. function BOARD_InitPeripherals)*/
extern void I2S_RX_eDMA_callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

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
