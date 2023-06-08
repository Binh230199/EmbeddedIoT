################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../THT_Libraries/Low\ Libraries/battery.c \
../THT_Libraries/Low\ Libraries/bq2429x.c \
../THT_Libraries/Low\ Libraries/fatfs_sd.c \
../THT_Libraries/Low\ Libraries/rainfall.c \
../THT_Libraries/Low\ Libraries/sd.c \
../THT_Libraries/Low\ Libraries/sim7600.c 

OBJS += \
./THT_Libraries/Low\ Libraries/battery.o \
./THT_Libraries/Low\ Libraries/bq2429x.o \
./THT_Libraries/Low\ Libraries/fatfs_sd.o \
./THT_Libraries/Low\ Libraries/rainfall.o \
./THT_Libraries/Low\ Libraries/sd.o \
./THT_Libraries/Low\ Libraries/sim7600.o 

C_DEPS += \
./THT_Libraries/Low\ Libraries/battery.d \
./THT_Libraries/Low\ Libraries/bq2429x.d \
./THT_Libraries/Low\ Libraries/fatfs_sd.d \
./THT_Libraries/Low\ Libraries/rainfall.d \
./THT_Libraries/Low\ Libraries/sd.d \
./THT_Libraries/Low\ Libraries/sim7600.d 


# Each subdirectory must supply rules for building sources it contributes
THT_Libraries/Low\ Libraries/battery.o: ../THT_Libraries/Low\ Libraries/battery.c THT_Libraries/Low\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Low Libraries/battery.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Low\ Libraries/bq2429x.o: ../THT_Libraries/Low\ Libraries/bq2429x.c THT_Libraries/Low\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Low Libraries/bq2429x.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Low\ Libraries/fatfs_sd.o: ../THT_Libraries/Low\ Libraries/fatfs_sd.c THT_Libraries/Low\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Low Libraries/fatfs_sd.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Low\ Libraries/rainfall.o: ../THT_Libraries/Low\ Libraries/rainfall.c THT_Libraries/Low\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Low Libraries/rainfall.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Low\ Libraries/sd.o: ../THT_Libraries/Low\ Libraries/sd.c THT_Libraries/Low\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Low Libraries/sd.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Low\ Libraries/sim7600.o: ../THT_Libraries/Low\ Libraries/sim7600.c THT_Libraries/Low\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Low Libraries/sim7600.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-THT_Libraries-2f-Low-20-Libraries

clean-THT_Libraries-2f-Low-20-Libraries:
	-$(RM) ./THT_Libraries/Low\ Libraries/battery.d ./THT_Libraries/Low\ Libraries/battery.o ./THT_Libraries/Low\ Libraries/bq2429x.d ./THT_Libraries/Low\ Libraries/bq2429x.o ./THT_Libraries/Low\ Libraries/fatfs_sd.d ./THT_Libraries/Low\ Libraries/fatfs_sd.o ./THT_Libraries/Low\ Libraries/rainfall.d ./THT_Libraries/Low\ Libraries/rainfall.o ./THT_Libraries/Low\ Libraries/sd.d ./THT_Libraries/Low\ Libraries/sd.o ./THT_Libraries/Low\ Libraries/sim7600.d ./THT_Libraries/Low\ Libraries/sim7600.o

.PHONY: clean-THT_Libraries-2f-Low-20-Libraries

