################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../device/system_MK64F12.c 

OBJS += \
./device/system_MK64F12.o 

C_DEPS += \
./device/system_MK64F12.d 


# Each subdirectory must supply rules for building sources it contributes
device/%.o: ../device/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\augus\Documents\MCUXpressoIDE_11.4.0_6224\workspace\DSP_TP4_Parte_1\board" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.4.0_6224\workspace\DSP_TP4_Parte_1\source" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.4.0_6224\workspace\DSP_TP4_Parte_1\drivers" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.4.0_6224\workspace\DSP_TP4_Parte_1\CMSIS\DSP\Include" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.4.0_6224\workspace\DSP_TP4_Parte_1\utilities" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.4.0_6224\workspace\DSP_TP4_Parte_1\component\serial_manager" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.4.0_6224\workspace\DSP_TP4_Parte_1\component\uart" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.4.0_6224\workspace\DSP_TP4_Parte_1\component\lists" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.4.0_6224\workspace\DSP_TP4_Parte_1\CMSIS" -I"C:\Users\augus\Documents\MCUXpressoIDE_11.4.0_6224\workspace\DSP_TP4_Parte_1\device" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


