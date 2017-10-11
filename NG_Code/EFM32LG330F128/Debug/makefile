################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/RTOS/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_LIST += \
EFM32LG330F128.eLST \

SECONDARY_SIZE += \
EFM32LG330F128.siz \


# All Target
all: EFM32LG330F128.elf secondary-outputs

# Tool invocations
EFM32LG330F128.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ThunderBench ARM C Linker'
	"C:\Program Files (x86)\EMPROG\ThunderBench-ARM-v3.85\IDE\\..\ARM\bin\arm-thunder-eabi-gcc" -Xlinker --gc-sections -static -T"U:\Thunderspace\EFM32LG330F128\EFM32LG330F128_flash.ld" -Wl,-Map,EFM32LG330F128.map -Xlinker --cref -mcpu=cortex-m3 -mthumb --specs=nano.specs -mfloat-abi=soft -g -gdwarf-4 -o "EFM32LG330F128.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

EFM32LG330F128.eLST: EFM32LG330F128.elf
	@echo 'Creating extended listing (.eLST)'
	"C:\Program Files (x86)\EMPROG\ThunderBench-ARM-v3.85\IDE\\..\ARM\bin\arm-thunder-eabi-objdump" -h -S EFM32LG330F128.elf > "EFM32LG330F128.eLST"
	@echo 'Finished building: $@'
	@echo ' '

EFM32LG330F128.siz: EFM32LG330F128.elf
	@echo 'Calculating project Code and Data sizes'
	"C:\Program Files (x86)\EMPROG\ThunderBench-ARM-v3.85\IDE\\..\ARM\bin\arm-thunder-eabi-size"  --format=berkeley EFM32LG330F128.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(C_DEPS) EFM32LG330F128.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
