/*
 * TRABAJO FINAL DSP (FCEFYN, UNC)
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
 * 	 I2S:
 * 	  -MCLK : J1[7]
 * 	  -RX_BCLK : J1[9]
 * 	  -RX_D : J1[15]
 * 	  -RX_FS : J1[13]
 *
 * 	TWR-ELEV:
 * 	 I2C:
 * 	  -SDA : J8[8]
 * 	  -SCLK : J8[7]
 * 	 I2S:
 * 	  -MCLK: J8[21]
 * 	  -BLCK: J8[22]
 * 	  -RX_D: J8[24]
 *	  -FS:   J8[23]
 *
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
#include "arm_math.h"

#endif /* MAIN_INCLUDE */

/*-------------------OTHER INCLUDES---------------*/
#include "sgtl5000.h"

/*-------------------DEFINES----------------------*/
#define BUFF_LEN 512U
#define BLOCKSIZE BUFF_LEN/4
#define NUMTAPS 12U

#define HALF_RANGE_Q31 2147483647.0f
#define FLOAT_TO_Q31(arg) (q31_t) (HALF_RANGE_Q31 * arg)
#define MAX_RANGE_Q31 (q31_t) 0x7FFFFFFF
#define MAX_RANGE_UINT32 0xFFFFFFFF

#define DELAY_DEPTH 32000U
#define MAX_DELAY 1000.0f // [ms]
#define MIN_DELAY 10U // [ms]
#define SAMPLE_RATE 32000.0f

#define IIR_FILTER_ALPHA 0.99f // 0 <= alpha <= 1

#define MIN_DELAY_FB (q31_t) 0x88508501

/*-------------------ENUM & STRUCT----------------*/
typedef enum {
	kPING,
	kPONG,
} buff_to_process;

typedef enum {
	kLockConfig,
	kConfigDelayAmount,
	kConfigDelayFb,
} delay_config_param;

typedef enum {
	kHomogeneousMix,
	kOnlySignal,
	kOnlyDelay,
} delay_mix;

typedef struct {
	float32_t  ;
	uint32_t out;
} iir_filter_struct;

/*---------------------VARIABLES------------------*/
q31_t pingPongIN[BUFF_LEN];
q31_t pingPongOUT[BUFF_LEN];

q31_t *pingIn = pingPongIN;
q31_t *pingOut = pingPongOUT;
q31_t *pongIn = &pingPongIN[BUFF_LEN/2];
q31_t *pongOut = &pingPongOUT[BUFF_LEN/2];

q31_t LInBuff[BLOCKSIZE], RInBuff[BLOCKSIZE];
q31_t LOutBuff[BLOCKSIZE], ROutBuff[BLOCKSIZE];

q31_t LInBuffScaledD[BLOCKSIZE];

int32_t readIndex = 0;
int32_t writeIndex = 0;
q31_t delayFeedBack = FLOAT_TO_Q31(0.0f);
uint32_t delayInMs = MIN_DELAY;
uint32_t delayInSamples = (uint32_t) ((float32_t) MIN_DELAY * SAMPLE_RATE / 1000.0f);
q31_t delayFifo[DELAY_DEPTH];

buff_to_process rx_proc_buffer, tx_proc_buffer;

volatile uint8_t rx_buffer_full = 0;
volatile uint8_t tx_buffer_empty = 0;

sai_transfer_t rxFer, txFer;

uint8_t syncFlag = 1;
uint8_t adcMux = 0;
delay_config_param configDelayParam = 0;
delay_mix configMix = 0;

uint16_t readValue;
uint32_t adcSampleCh0, adcSampleCh1;

iir_filter_struct iirFilt;
iir_filter_struct iirFilt2;

/*---------------FUNCTION PROTOTYPES--------------*/
void debug_printf_registers();
void process_block();
void cook_variables();
uint32_t single_pole_iir_filter();

/*---------------------TODO:MAIN------------------*/
int main(void) {
    /* Init board hardware. */
 	BOARD_InitBootPins();
	BOARD_InitButtonsPins();
	BOARD_InitLEDsPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    /*-----------------ALGORITHM BEGIN-----------------*/
    /* I2S struct init */
    rxFer.dataSize = sizeof(pingPongIN)/2;
    txFer.dataSize = sizeof(pingPongOUT)/2;

    rxFer.data = (uint8_t *) pingIn;
    txFer.data = (uint8_t *) pingOut;

    /* DELAY Setup */
    readIndex = writeIndex - delayInSamples;
    if(readIndex < 0)
    	readIndex += DELAY_DEPTH;

    /* IIR filter setup */
    iirFilt.alpha = IIR_FILTER_ALPHA;
    iirFilt.out = 0U;

    iirFilt2.alpha = IIR_FILTER_ALPHA;
    iirFilt2.out = 0U;

    /* SGTL5000 codec init */
//    sgtl5000_init_Line_in_AVC_HP_out_32K();
//    sgtl5000_init_MIC_in_AVC_HP_out_32K();
    sgtl5000_init_Line_in_AVC_Line_out_32K();

    SAI_TransferReceiveEDMA(I2S0_PERIPHERAL, &I2S0_SAI_Rx_eDMA_Handle, &rxFer);

    PIT_StartTimer(PIT_PERIPHERAL, kPIT_Chnl_0);

    while(1) {
    	while (!(rx_buffer_full && tx_buffer_empty)) {}

    	GPIO_PinWrite(BOARD_GPIO_pin_GPIO, BOARD_GPIO_pin_PIN, 1U);
    	process_block();
    	GPIO_PinWrite(BOARD_GPIO_pin_GPIO, BOARD_GPIO_pin_PIN, 0U);

    	if(configDelayParam == kConfigDelayAmount)
    		PIT_SetTimerPeriod(PIT_PERIPHERAL, kPIT_Chnl_0, MSEC_TO_COUNT(delayInMs >> 1U, PIT_CLK_FREQ));
    }

    return 0;
}

uint32_t single_pole_iir_filter(iir_filter_struct *filter, uint32_t in) {
	filter->out = (uint32_t) ((1.0f - filter->alpha) * (float32_t) in + (filter->alpha) * (float32_t) filter->out);

	return filter->out;
}

void cook_variables() {
	switch (configDelayParam) {
	case kConfigDelayAmount:
		delayInMs = (uint32_t) ((float32_t) adcSampleCh0 * MAX_DELAY/4095.0f);
		if(delayInMs < MIN_DELAY)
			delayInMs = MIN_DELAY;

		delayInSamples = (uint32_t) ((float32_t) delayInMs * SAMPLE_RATE/1000.0f);

		readIndex = writeIndex - delayInSamples;
		if(readIndex < 0)
			readIndex += DELAY_DEPTH;
		break;
	case kConfigDelayFb:
		delayFeedBack =  (q31_t) (adcSampleCh1 * 1048832) - (q31_t) HALF_RANGE_Q31;
		if(delayFeedBack < MIN_DELAY_FB)
			delayFeedBack = MIN_DELAY_FB;
		break;
	case kLockConfig:
	default:
		break;
	}
}

void process_block() {
	q31_t *pRx, *pTx;

	if(rx_proc_buffer == kPING)
		pRx = pingIn;
	else
		pRx = pongIn;

	if(tx_proc_buffer == kPING)
		pTx = pingOut;
	else
		pTx = pongOut;

	/* Separate L and R channel samples */
	for(uint16_t i = 0U; i < BLOCKSIZE; i++) {
		RInBuff[i] = *pRx++;
		LInBuff[i] = *pRx++;
	}

	/*
	 * Apply Digital Effect
	 * TODO:
	 * Por ahora solo L channel
	 * WET/DRY 50/50 unicamente
	 */

	cook_variables();

	/* Scale down input samples */
	arm_scale_q31(LInBuff, MAX_RANGE_Q31, -1, LInBuffScaledD, BLOCKSIZE); // scale = scaleFract * 2^shift

	for(uint16_t i = 0U; i < BLOCKSIZE; i++) {
		/* Read the output of the delay at readIndex */
		q31_t y = delayFifo[readIndex];
		q31_t aux;

		/* Write the input to the delay */
		arm_mult_q31(&delayFeedBack, &y, &aux, 1U);
		arm_add_q31(&aux, &LInBuffScaledD[i], &delayFifo[writeIndex], 1U); // x + delayFeedBack*y;

		/* Create the wet/dry mix and write to the output buffer */
		switch (configMix) {
		case kHomogeneousMix:
			arm_add_q31(&y, &LInBuffScaledD[i], pTx, 1U);
			break;
		case kOnlySignal:
			arm_scale_q31(&LInBuffScaledD[i], FLOAT_TO_Q31(0.9f), 1U, pTx, 1U);
			break;
		case kOnlyDelay:
			arm_scale_q31(&y, FLOAT_TO_Q31(0.9f), 1U, pTx, 1U);
			break;
		default:
			arm_add_q31(&y, &LInBuffScaledD[i], pTx, 1U);
			break;
		}

		/* Copy L samples in R channel */
		arm_copy_q31(pTx, (pTx + 1), 1U);
		pTx = pTx + 2;

		/* Increment the pointers and wrap if necessary */
		writeIndex++;
		if(writeIndex >= DELAY_DEPTH)
			writeIndex = 0;
		readIndex++;
		if(readIndex >= DELAY_DEPTH)
			readIndex = 0;
	}

	rx_buffer_full = 0;
	tx_buffer_empty = 0;
}

void I2S_RX_eDMA_callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData) {
	if(rxFer.data == (uint8_t *) pingIn) {
		rxFer.data = (uint8_t *) pongIn;
		rx_proc_buffer = kPING;
	}

	else {
		rxFer.data = (uint8_t *) pingIn;
		rx_proc_buffer = kPONG;
	}

//	GPIO_PortToggle(BOARD_GPIO_pin_GPIO, 1 << BOARD_GPIO_pin_PIN);
	rx_buffer_full = 1;
	SAI_TransferReceiveEDMA(I2S0_PERIPHERAL, &I2S0_SAI_Rx_eDMA_Handle, &rxFer);

	if (syncFlag) {
		SAI_TransferSendEDMA(I2S0_PERIPHERAL, &I2S0_SAI_Tx_eDMA_Handle, &txFer);
		syncFlag = 0;
	}

}

void I2S_TX_eDMA_callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData) {
	if(txFer.data == (uint8_t *) pingOut) {
		txFer.data = (uint8_t *) pongOut;
		tx_proc_buffer = kPING;
	}

	else {
		txFer.data = (uint8_t *) pingOut;
		tx_proc_buffer = kPONG;
	}

//	GPIO_PortToggle(BOARD_GPIO_pin_GPIO, 1 << BOARD_GPIO_pin_PIN);
	tx_buffer_empty = 1;
	SAI_TransferSendEDMA(I2S0_PERIPHERAL, &I2S0_SAI_Tx_eDMA_Handle, &txFer);
}

void debug_printf_registers() {
	//PowerUp
	readValue = sgtl5000_read_register(SGTL5000_CHIP_LINREG_CTRL);
	PRINTF("get sgtl5000 SGTL5000_CHIP_LINREG_CTRL: %x\n", readValue);

	readValue = sgtl5000_read_register(SGTL5000_CHIP_ANA_POWER);
	PRINTF("get sgtl5000 SGTL5000_CHIP_ANA_POWER: %x\n", readValue);

	readValue = sgtl5000_read_register(SGTL5000_CHIP_DIG_POWER);
	PRINTF("get sgtl5000 SGTL5000_CHIP_DIG_POWER: %x\n", readValue);

	// IO Routing
	readValue = sgtl5000_read_register(SGTL5000_CHIP_SSS_CTRL);
	PRINTF("get sgtl5000 SGTL5000_CHIP_SSS_CTRL: %x\n", readValue);

	// Volume
	readValue = sgtl5000_read_register(SGTL5000_CHIP_MIC_CTRL);
	PRINTF("get sgtl5000 SGTL5000_CHIP_MIC_CTRL: %x\n", readValue);

	readValue = sgtl5000_read_register(SGTL5000_CHIP_ANA_CTRL);
	PRINTF("get sgtl5000 SGTL5000_CHIP_ANA_CTRL: %x\n", readValue);

	readValue = sgtl5000_read_register(SGTL5000_CHIP_ANA_HP_CTRL);
	PRINTF("get sgtl5000 SGTL5000_CHIP_ANA_HP_CTRL: %x\n", readValue);

	readValue = sgtl5000_read_register(SGTL5000_CHIP_ANA_STATUS);
	PRINTF("get sgtl5000 SGTL5000_CHIP_ANA_STATUS: %x\n", readValue);

	readValue = sgtl5000_read_register(SGTL5000_CHIP_ADCDAC_CTRL);
	PRINTF("get sgtl5000 SGTL5000_CHIP_ADCDAC_CTRL: %x\n", readValue);

	readValue = sgtl5000_read_register(SGTL5000_CHIP_DAC_VOL);
	PRINTF("get sgtl5000 SGTL5000_CHIP_DAC_VOL: %x\n", readValue);
}

/* ADC0_IRQn interrupt handler */
void ADC0_IRQHANDLER(void) {
  /* Array of result values*/
  uint32_t result_values[2] = {0};

  /* Get flags for each group */
  for ( int i=0; i<2; i++){
  uint32_t status = ADC16_GetChannelStatusFlags(ADC0_PERIPHERAL, i);
  	if ( status == kADC16_ChannelConversionDoneFlag){
  		result_values[i] = ADC16_GetChannelConversionValue(ADC0_PERIPHERAL, i);
  	}
  }

  /* Place your code here */
  if(!adcMux) {
	  adcSampleCh0 = result_values[0];
  	  adcSampleCh0 = single_pole_iir_filter(&iirFilt, adcSampleCh0);
  }

  else {
	  adcSampleCh1 = result_values[0];
	  adcSampleCh1 = single_pole_iir_filter(&iirFilt2, adcSampleCh1);
  }

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}

/* PIT0_IRQn interrupt handler */
void PIT_CHANNEL_0_IRQHANDLER(void) {
  uint32_t intStatus;
  /* Reading all interrupt flags of status register */
  intStatus = PIT_GetStatusFlags(PIT_PERIPHERAL, PIT_CHANNEL_0);
  PIT_ClearStatusFlags(PIT_PERIPHERAL, PIT_CHANNEL_0, intStatus);

  /* Place your code here */
  switch (configDelayParam) {
  case kLockConfig:
	  LED_RED_OFF();
	  LED_GREEN_TOGGLE();
	  break;
  case kConfigDelayAmount:
	  LED_GREEN_OFF();
	  LED_RED_TOGGLE();
	  break;
  default:
	  break;
  }

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}

/* PIT1_IRQn interrupt handler */
void PIT_CHANNEL_1_IRQHANDLER(void) {
  uint32_t intStatus;
  static uint16_t counter = 0U;

   /* Reading all interrupt flags of status register */
  intStatus = PIT_GetStatusFlags(PIT_PERIPHERAL, PIT_CHANNEL_1);
  PIT_ClearStatusFlags(PIT_PERIPHERAL, PIT_CHANNEL_1, intStatus);

  /* Place your code here */
  adcMux ^= 0b1;
  ADC16_SetChannelConfig(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP, &ADC0_channelsConfig[adcMux]);

  if(configDelayParam == kConfigDelayFb) {
	  uint16_t aux = (uint16_t) ((float32_t) adcSampleCh1/4095.0f * 20.0f);

	  if(aux >= 10U) {
		  if(counter < aux - 10U)
			  LED_RED_ON();
		  else
			  LED_RED_OFF();
	  }

	  else {
		  if(counter < aux)
			  LED_RED_OFF();
		  else
			  LED_RED_ON();
	  }
  }

  if(counter == 10U)
	  counter = 0U;
  else
	  counter++;

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}

/* PORTC_IRQn interrupt handler */
void GPIOC_IRQHANDLER(void) {
  /* Get pin flags */
  uint32_t pin_flags = GPIO_PortGetInterruptFlags(GPIOC);

  /* Place your interrupt code here */
  if(configDelayParam == kConfigDelayFb)
	  configDelayParam = kLockConfig;
  else
	  configDelayParam++;

  /* Clear pin flags */
  GPIO_PortClearInterruptFlags(GPIOC, pin_flags);

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}

/* PORTA_IRQn interrupt handler */
void GPIOA_IRQHANDLER(void) {
  /* Get pin flags */
  uint32_t pin_flags = GPIO_PortGetInterruptFlags(GPIOA);

  /* Place your interrupt code here */
  if(configMix == kOnlyDelay)
	  configMix = kHomogeneousMix;
  else
	  configMix++;

  /* Clear pin flags */
  GPIO_PortClearInterruptFlags(GPIOA, pin_flags);

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}

