################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Third_Party/lwshell/src/lwshell/lwshell.c \
../Third_Party/lwshell/src/lwshell/lwshell_user.c 

OBJS += \
./Third_Party/lwshell/src/lwshell/lwshell.o \
./Third_Party/lwshell/src/lwshell/lwshell_user.o 

C_DEPS += \
./Third_Party/lwshell/src/lwshell/lwshell.d \
./Third_Party/lwshell/src/lwshell/lwshell_user.d 


# Each subdirectory must supply rules for building sources it contributes
Third_Party/lwshell/src/lwshell/%.o Third_Party/lwshell/src/lwshell/%.su Third_Party/lwshell/src/lwshell/%.cyclo: ../Third_Party/lwshell/src/lwshell/%.c Third_Party/lwshell/src/lwshell/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/lwrb/src/include" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/lwprintf/src/include" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/lwshell/src/include" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/MultiButton" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/ee24/inc" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/spif/inc" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/OSEK/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Third_Party-2f-lwshell-2f-src-2f-lwshell

clean-Third_Party-2f-lwshell-2f-src-2f-lwshell:
	-$(RM) ./Third_Party/lwshell/src/lwshell/lwshell.cyclo ./Third_Party/lwshell/src/lwshell/lwshell.d ./Third_Party/lwshell/src/lwshell/lwshell.o ./Third_Party/lwshell/src/lwshell/lwshell.su ./Third_Party/lwshell/src/lwshell/lwshell_user.cyclo ./Third_Party/lwshell/src/lwshell/lwshell_user.d ./Third_Party/lwshell/src/lwshell/lwshell_user.o ./Third_Party/lwshell/src/lwshell/lwshell_user.su

.PHONY: clean-Third_Party-2f-lwshell-2f-src-2f-lwshell

