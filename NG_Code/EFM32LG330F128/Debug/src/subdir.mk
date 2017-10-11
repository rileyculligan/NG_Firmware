################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ADC_Module.c \
../src/LED_Module.c \
../src/main.c \
../src/startup_efm32lg.c \
../src/system_efm32lg.c 

OBJS += \
./src/ADC_Module.o \
./src/LED_Module.o \
./src/main.o \
./src/startup_efm32lg.o \
./src/system_efm32lg.o 

C_DEPS += \
./src/ADC_Module.d \
./src/LED_Module.d \
./src/main.d \
./src/startup_efm32lg.d \
./src/system_efm32lg.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ThunderBench ARM C Compiler'
	"C:\Program Files (x86)\EMPROG\ThunderBench-ARM-v3.85\IDE\\..\ARM\bin\arm-thunder-eabi-gcc" -DEFM32LG330F128 -D__GNUC__=4 -I"U:\Thunderspace\EFM32LG330F128\Libraries\Device\EnergyMicro\EFM32\Include" -I"U:\Thunderspace\EFM32LG330F128\Libraries\CMSIS\Include" -I"U:\Thunderspace\EFM32LG330F128\src" -I"U:/Thunderspace/EFM32LG330F128/RTOSincludes" -O0 -ffunction-sections -fdata-sections -Wall -std=gnu99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb --specs=nano.specs -mfloat-abi=soft -g -gdwarf-4 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


