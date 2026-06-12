################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/libraries/MQ-2/Gas.c 

OBJS += \
./Core/libraries/MQ-2/Gas.o 

C_DEPS += \
./Core/libraries/MQ-2/Gas.d 


# Each subdirectory must supply rules for building sources it contributes
Core/libraries/MQ-2/%.o Core/libraries/MQ-2/%.su Core/libraries/MQ-2/%.cyclo: ../Core/libraries/MQ-2/%.c Core/libraries/MQ-2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I"F:/BaiTap/HTCB/Hethongcambienv3/Drivers/CMSIS/RTOS2/Include" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-libraries-2f-MQ-2d-2

clean-Core-2f-libraries-2f-MQ-2d-2:
	-$(RM) ./Core/libraries/MQ-2/Gas.cyclo ./Core/libraries/MQ-2/Gas.d ./Core/libraries/MQ-2/Gas.o ./Core/libraries/MQ-2/Gas.su

.PHONY: clean-Core-2f-libraries-2f-MQ-2d-2

