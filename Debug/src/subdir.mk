################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/\ ButtonTask.c \
../src/LedTask.c \
../src/main.c \
../src/tasks.c 

OBJS += \
./src/\ ButtonTask.o \
./src/LedTask.o \
./src/main.o \
./src/tasks.o 

C_DEPS += \
./src/\ ButtonTask.d \
./src/LedTask.d \
./src/main.d \
./src/tasks.d 


# Each subdirectory must supply rules for building sources it contributes
src/\ ButtonTask.o: ../src/\ ButtonTask.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -DDEBUG -I"../libs/misc/include" -I"../include" -I"../libs/CMSIS/include" -I"../libs/StdPeriph/include" -I"/Users/felipeneves/Documents/Projects/Embedded/uLipeRTOS/RTOS" -std=gnu11 -MMD -MP -MF"src/ ButtonTask.d" -MT"src/\ ButtonTask.d" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -DDEBUG -I"../libs/misc/include" -I"../include" -I"../libs/CMSIS/include" -I"../libs/StdPeriph/include" -I"/Users/felipeneves/Documents/Projects/Embedded/uLipeRTOS/RTOS" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


