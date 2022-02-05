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
-include component/timer_manager/subdir.mk
-include component/timer/subdir.mk
-include component/serial_manager/subdir.mk
-include component/lists/subdir.mk
-include component/gpio/subdir.mk
-include component/button/subdir.mk
-include board/subdir.mk
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
all: TP2_DSP.axf

# Tool invocations
TP2_DSP.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\augus\Documents\MCUXpressoIDE_11.2.0_4120\workspace\TP2_DSP\libs" -Xlinker -Map="TP2_DSP.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -T MK64FN1M0xxx12_Project_Debug.ld -o "TP2_DSP.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) TP2_DSP.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "TP2_DSP.axf"; # arm-none-eabi-objcopy -v -O binary "TP2_DSP.axf" "TP2_DSP.bin" ; # checksum -p MK64FN1M0xxx12 -d "TP2_DSP.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
