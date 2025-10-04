################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Third_Party/lwprintf/src/system/lwprintf_sys_cmsis_os.c \
../Third_Party/lwprintf/src/system/lwprintf_sys_threadx.c \
../Third_Party/lwprintf/src/system/lwprintf_sys_win32.c 

OBJS += \
./Third_Party/lwprintf/src/system/lwprintf_sys_cmsis_os.o \
./Third_Party/lwprintf/src/system/lwprintf_sys_threadx.o \
./Third_Party/lwprintf/src/system/lwprintf_sys_win32.o 

C_DEPS += \
./Third_Party/lwprintf/src/system/lwprintf_sys_cmsis_os.d \
./Third_Party/lwprintf/src/system/lwprintf_sys_threadx.d \
./Third_Party/lwprintf/src/system/lwprintf_sys_win32.d 


# Each subdirectory must supply rules for building sources it contributes
Third_Party/lwprintf/src/system/%.o Third_Party/lwprintf/src/system/%.su Third_Party/lwprintf/src/system/%.cyclo: ../Third_Party/lwprintf/src/system/%.c Third_Party/lwprintf/src/system/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/lwrb/src/include" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/lwprintf/src/include" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/lwshell/src/include" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/MultiButton" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/ee24/inc" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/spif/inc" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/OSEK/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Third_Party-2f-lwprintf-2f-src-2f-system

clean-Third_Party-2f-lwprintf-2f-src-2f-system:
	-$(RM) ./Third_Party/lwprintf/src/system/lwprintf_sys_cmsis_os.cyclo ./Third_Party/lwprintf/src/system/lwprintf_sys_cmsis_os.d ./Third_Party/lwprintf/src/system/lwprintf_sys_cmsis_os.o ./Third_Party/lwprintf/src/system/lwprintf_sys_cmsis_os.su ./Third_Party/lwprintf/src/system/lwprintf_sys_threadx.cyclo ./Third_Party/lwprintf/src/system/lwprintf_sys_threadx.d ./Third_Party/lwprintf/src/system/lwprintf_sys_threadx.o ./Third_Party/lwprintf/src/system/lwprintf_sys_threadx.su ./Third_Party/lwprintf/src/system/lwprintf_sys_win32.cyclo ./Third_Party/lwprintf/src/system/lwprintf_sys_win32.d ./Third_Party/lwprintf/src/system/lwprintf_sys_win32.o ./Third_Party/lwprintf/src/system/lwprintf_sys_win32.su

.PHONY: clean-Third_Party-2f-lwprintf-2f-src-2f-system

