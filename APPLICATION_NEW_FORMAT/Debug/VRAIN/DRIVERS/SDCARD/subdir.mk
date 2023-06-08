################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../VRAIN/DRIVERS/SDCARD/fatfs_sd.c \
../VRAIN/DRIVERS/SDCARD/sd.c 

OBJS += \
./VRAIN/DRIVERS/SDCARD/fatfs_sd.o \
./VRAIN/DRIVERS/SDCARD/sd.o 

C_DEPS += \
./VRAIN/DRIVERS/SDCARD/fatfs_sd.d \
./VRAIN/DRIVERS/SDCARD/sd.d 


# Each subdirectory must supply rules for building sources it contributes
VRAIN/DRIVERS/SDCARD/%.o: ../VRAIN/DRIVERS/SDCARD/%.c VRAIN/DRIVERS/SDCARD/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-VRAIN-2f-DRIVERS-2f-SDCARD

clean-VRAIN-2f-DRIVERS-2f-SDCARD:
	-$(RM) ./VRAIN/DRIVERS/SDCARD/fatfs_sd.d ./VRAIN/DRIVERS/SDCARD/fatfs_sd.o ./VRAIN/DRIVERS/SDCARD/sd.d ./VRAIN/DRIVERS/SDCARD/sd.o

.PHONY: clean-VRAIN-2f-DRIVERS-2f-SDCARD

