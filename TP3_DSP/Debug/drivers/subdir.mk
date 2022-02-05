################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_adc16.c \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_gpio.c \
../drivers/fsl_i2c.c \
../drivers/fsl_pit.c \
../drivers/fsl_smc.c \
../drivers/fsl_uart.c 

OBJS += \
./drivers/fsl_adc16.o \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_gpio.o \
./drivers/fsl_i2c.o \
./drivers/fsl_pit.o \
./drivers/fsl_smc.o \
./drivers/fsl_uart.o 

C_DEPS += \
./drivers/fsl_adc16.d \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_gpio.d \
./drivers/fsl_i2c.d \
./drivers/fsl_pit.d \
./drivers/fsl_smc.d \
./drivers/fsl_uart.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP3_DSP\board" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP3_DSP\source" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP3_DSP" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP3_DSP\drivers" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP3_DSP\device" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP3_DSP\CMSIS" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP3_DSP\component\uart" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP3_DSP\utilities" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP3_DSP\component\serial_manager" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP3_DSP\component\lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


