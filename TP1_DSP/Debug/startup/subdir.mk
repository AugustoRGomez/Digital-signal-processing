################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../startup/startup_mk64f12.c 

OBJS += \
./startup/startup_mk64f12.o 

C_DEPS += \
./startup/startup_mk64f12.d 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=1 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP1_DSP\drivers" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP1_DSP\CMSIS" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP1_DSP\component\uart" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP1_DSP\device" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP1_DSP\component\serial_manager" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP1_DSP\freemaster" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP1_DSP\component\lists" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP1_DSP\source" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP1_DSP\utilities" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP1_DSP\board" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP1_DSP" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


