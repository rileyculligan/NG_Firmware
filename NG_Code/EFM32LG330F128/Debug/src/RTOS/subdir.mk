################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/RTOS/heap_1.c \
../src/RTOS/list.c \
../src/RTOS/port.c \
../src/RTOS/queue.c \
../src/RTOS/tasks.c 

OBJS += \
./src/RTOS/heap_1.o \
./src/RTOS/list.o \
./src/RTOS/port.o \
./src/RTOS/queue.o \
./src/RTOS/tasks.o 

C_DEPS += \
./src/RTOS/heap_1.d \
./src/RTOS/list.d \
./src/RTOS/port.d \
./src/RTOS/queue.d \
./src/RTOS/tasks.d 


# Each subdirectory must supply rules for building sources it contributes
src/RTOS/%.o: ../src/RTOS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ThunderBench ARM C Compiler'
	"C:\Program Files (x86)\EMPROG\ThunderBench-ARM-v3.85\IDE\\..\ARM\bin\arm-thunder-eabi-gcc" -DEFM32LG330F128 -D__GNUC__=4 -I"U:\Thunderspace\EFM32LG330F128\Libraries\Device\EnergyMicro\EFM32\Include" -I"U:\Thunderspace\EFM32LG330F128\Libraries\CMSIS\Include" -I"U:\Thunderspace\EFM32LG330F128\src" -I"U:/Thunderspace/EFM32LG330F128/RTOSincludes" -O0 -ffunction-sections -fdata-sections -Wall -std=gnu99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb --specs=nano.specs -mfloat-abi=soft -g -gdwarf-4 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


