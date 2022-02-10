################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include startup/subdir.mk
-include source/subdir.mk
-include drivers/subdir.mk
-include device/subdir.mk
-include component/uart/subdir.mk
-include component/serial_manager/subdir.mk
-include component/lists/subdir.mk
-include board/subdir.mk
-include CMSIS/DSP/Source/TransformFunctions/subdir.mk
-include CMSIS/DSP/Source/SupportFunctions/subdir.mk
-include CMSIS/DSP/Source/StatisticsFunctions/subdir.mk
-include CMSIS/DSP/Source/MatrixFunctions/subdir.mk
-include CMSIS/DSP/Source/FilteringFunctions/subdir.mk
-include CMSIS/DSP/Source/FastMathFunctions/subdir.mk
-include CMSIS/DSP/Source/ControllerFunctions/subdir.mk
-include CMSIS/DSP/Source/ComplexMathFunctions/subdir.mk
-include CMSIS/DSP/Source/CommonTables/subdir.mk
-include CMSIS/DSP/Source/BasicMathFunctions/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: DSP_TP4_Parte_2.axf

# Tool invocations
DSP_TP4_Parte_2.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\augus\Documents\MCUXpressoIDE_11.4.0_6224\workspace\DSP_TP4_Parte_2\CMSIS\DSP" -Xlinker -Map="DSP_TP4_Parte_2.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -T DSP_TP4_Parte_2_Debug.ld -o "DSP_TP4_Parte_2.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) DSP_TP4_Parte_2.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "DSP_TP4_Parte_2.axf"; # arm-none-eabi-objcopy -v -O binary "DSP_TP4_Parte_2.axf" "DSP_TP4_Parte_2.bin" ; # checksum -p MK64FN1M0xxx12 -d "DSP_TP4_Parte_2.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
