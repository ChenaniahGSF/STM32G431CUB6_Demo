################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../OSEK/src/OS.c \
../OSEK/src/OsAlarm.c \
../OSEK/src/OsAsm.c \
../OSEK/src/OsEvt.c \
../OSEK/src/OsTask.c \
../OSEK/src/SysTickTimer.c \
../OSEK/src/TCB.c 

OBJS += \
./OSEK/src/OS.o \
./OSEK/src/OsAlarm.o \
./OSEK/src/OsAsm.o \
./OSEK/src/OsEvt.o \
./OSEK/src/OsTask.o \
./OSEK/src/SysTickTimer.o \
./OSEK/src/TCB.o 

C_DEPS += \
./OSEK/src/OS.d \
./OSEK/src/OsAlarm.d \
./OSEK/src/OsAsm.d \
./OSEK/src/OsEvt.d \
./OSEK/src/OsTask.d \
./OSEK/src/SysTickTimer.d \
./OSEK/src/TCB.d 


# Each subdirectory must supply rules for building sources it contributes
OSEK/src/%.o OSEK/src/%.su OSEK/src/%.cyclo: ../OSEK/src/%.c OSEK/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/lwrb/src/include" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/lwprintf/src/include" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/lwshell/src/include" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/MultiButton" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/ee24/inc" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/Third_Party/spif/inc" -I"D:/Develop/STM32/STM32G431CUB6/STM32G431CUB6_Demo/OSEK/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-OSEK-2f-src

clean-OSEK-2f-src:
	-$(RM) ./OSEK/src/OS.cyclo ./OSEK/src/OS.d ./OSEK/src/OS.o ./OSEK/src/OS.su ./OSEK/src/OsAlarm.cyclo ./OSEK/src/OsAlarm.d ./OSEK/src/OsAlarm.o ./OSEK/src/OsAlarm.su ./OSEK/src/OsAsm.cyclo ./OSEK/src/OsAsm.d ./OSEK/src/OsAsm.o ./OSEK/src/OsAsm.su ./OSEK/src/OsEvt.cyclo ./OSEK/src/OsEvt.d ./OSEK/src/OsEvt.o ./OSEK/src/OsEvt.su ./OSEK/src/OsTask.cyclo ./OSEK/src/OsTask.d ./OSEK/src/OsTask.o ./OSEK/src/OsTask.su ./OSEK/src/SysTickTimer.cyclo ./OSEK/src/SysTickTimer.d ./OSEK/src/SysTickTimer.o ./OSEK/src/SysTickTimer.su ./OSEK/src/TCB.cyclo ./OSEK/src/TCB.d ./OSEK/src/TCB.o ./OSEK/src/TCB.su

.PHONY: clean-OSEK-2f-src

