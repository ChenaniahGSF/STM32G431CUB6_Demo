################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Third_Party/lwrb/src/lwrb/lwrb.c \
../Third_Party/lwrb/src/lwrb/lwrb_ex.c 

OBJS += \
./Third_Party/lwrb/src/lwrb/lwrb.o \
./Third_Party/lwrb/src/lwrb/lwrb_ex.o 

C_DEPS += \
./Third_Party/lwrb/src/lwrb/lwrb.d \
./Third_Party/lwrb/src/lwrb/lwrb_ex.d 


# Each subdirectory must supply rules for building sources it contributes
Third_Party/lwrb/src/lwrb/%.o Third_Party/lwrb/src/lwrb/%.su Third_Party/lwrb/src/lwrb/%.cyclo: ../Third_Party/lwrb/src/lwrb/%.c Third_Party/lwrb/src/lwrb/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/lwrb/src/include" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/lwprintf/src/include" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/lwshell/src/include" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/MultiButton" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/ee24/inc" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/spif/inc" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/OSEK/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Third_Party-2f-lwrb-2f-src-2f-lwrb

clean-Third_Party-2f-lwrb-2f-src-2f-lwrb:
	-$(RM) ./Third_Party/lwrb/src/lwrb/lwrb.cyclo ./Third_Party/lwrb/src/lwrb/lwrb.d ./Third_Party/lwrb/src/lwrb/lwrb.o ./Third_Party/lwrb/src/lwrb/lwrb.su ./Third_Party/lwrb/src/lwrb/lwrb_ex.cyclo ./Third_Party/lwrb/src/lwrb/lwrb_ex.d ./Third_Party/lwrb/src/lwrb/lwrb_ex.o ./Third_Party/lwrb/src/lwrb/lwrb_ex.su

.PHONY: clean-Third_Party-2f-lwrb-2f-src-2f-lwrb

