################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/timer_manager/timer_manager.c 

OBJS += \
./component/timer_manager/timer_manager.o 

C_DEPS += \
./component/timer_manager/timer_manager.d 


# Each subdirectory must supply rules for building sources it contributes
component/timer_manager/%.o: ../component/timer_manager/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\board" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\source" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\drivers" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\device" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\CMSIS" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\component\uart" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\utilities" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\component\serial_manager" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\component\lists" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\freemaster" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\component\timer" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\component\gpio" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\component\timer_manager" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\component\button" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


