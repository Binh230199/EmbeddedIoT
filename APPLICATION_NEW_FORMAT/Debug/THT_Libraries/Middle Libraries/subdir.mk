################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../THT_Libraries/Middle\ Libraries/clock.c \
../THT_Libraries/Middle\ Libraries/dev_config.c \
../THT_Libraries/Middle\ Libraries/fota.c \
../THT_Libraries/Middle\ Libraries/http.c \
../THT_Libraries/Middle\ Libraries/level_sensor.c \
../THT_Libraries/Middle\ Libraries/power.c \
../THT_Libraries/Middle\ Libraries/sdcard.c \
../THT_Libraries/Middle\ Libraries/sim.c \
../THT_Libraries/Middle\ Libraries/sms.c \
../THT_Libraries/Middle\ Libraries/telegram.c \
../THT_Libraries/Middle\ Libraries/wlv_def.c 

OBJS += \
./THT_Libraries/Middle\ Libraries/clock.o \
./THT_Libraries/Middle\ Libraries/dev_config.o \
./THT_Libraries/Middle\ Libraries/fota.o \
./THT_Libraries/Middle\ Libraries/http.o \
./THT_Libraries/Middle\ Libraries/level_sensor.o \
./THT_Libraries/Middle\ Libraries/power.o \
./THT_Libraries/Middle\ Libraries/sdcard.o \
./THT_Libraries/Middle\ Libraries/sim.o \
./THT_Libraries/Middle\ Libraries/sms.o \
./THT_Libraries/Middle\ Libraries/telegram.o \
./THT_Libraries/Middle\ Libraries/wlv_def.o 

C_DEPS += \
./THT_Libraries/Middle\ Libraries/clock.d \
./THT_Libraries/Middle\ Libraries/dev_config.d \
./THT_Libraries/Middle\ Libraries/fota.d \
./THT_Libraries/Middle\ Libraries/http.d \
./THT_Libraries/Middle\ Libraries/level_sensor.d \
./THT_Libraries/Middle\ Libraries/power.d \
./THT_Libraries/Middle\ Libraries/sdcard.d \
./THT_Libraries/Middle\ Libraries/sim.d \
./THT_Libraries/Middle\ Libraries/sms.d \
./THT_Libraries/Middle\ Libraries/telegram.d \
./THT_Libraries/Middle\ Libraries/wlv_def.d 


# Each subdirectory must supply rules for building sources it contributes
THT_Libraries/Middle\ Libraries/clock.o: ../THT_Libraries/Middle\ Libraries/clock.c THT_Libraries/Middle\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Middle Libraries/clock.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Middle\ Libraries/dev_config.o: ../THT_Libraries/Middle\ Libraries/dev_config.c THT_Libraries/Middle\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Middle Libraries/dev_config.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Middle\ Libraries/fota.o: ../THT_Libraries/Middle\ Libraries/fota.c THT_Libraries/Middle\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Middle Libraries/fota.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Middle\ Libraries/http.o: ../THT_Libraries/Middle\ Libraries/http.c THT_Libraries/Middle\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Middle Libraries/http.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Middle\ Libraries/level_sensor.o: ../THT_Libraries/Middle\ Libraries/level_sensor.c THT_Libraries/Middle\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Middle Libraries/level_sensor.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Middle\ Libraries/power.o: ../THT_Libraries/Middle\ Libraries/power.c THT_Libraries/Middle\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Middle Libraries/power.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Middle\ Libraries/sdcard.o: ../THT_Libraries/Middle\ Libraries/sdcard.c THT_Libraries/Middle\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Middle Libraries/sdcard.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Middle\ Libraries/sim.o: ../THT_Libraries/Middle\ Libraries/sim.c THT_Libraries/Middle\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Middle Libraries/sim.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Middle\ Libraries/sms.o: ../THT_Libraries/Middle\ Libraries/sms.c THT_Libraries/Middle\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Middle Libraries/sms.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Middle\ Libraries/telegram.o: ../THT_Libraries/Middle\ Libraries/telegram.c THT_Libraries/Middle\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Middle Libraries/telegram.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
THT_Libraries/Middle\ Libraries/wlv_def.o: ../THT_Libraries/Middle\ Libraries/wlv_def.c THT_Libraries/Middle\ Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I"../THT_Libraries/Low Libraries" -I"../THT_Libraries/Middle Libraries" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/CONFIG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FOTA" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/APPLICATION/FUNCTIONS" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/BQ2429x" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SDCARD" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/LOG" -I"C:/Users/This PC/STM32CubeIDE/VER_100/APPLICATION/VRAIN/DRIVERS/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"THT_Libraries/Middle Libraries/wlv_def.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-THT_Libraries-2f-Middle-20-Libraries

clean-THT_Libraries-2f-Middle-20-Libraries:
	-$(RM) ./THT_Libraries/Middle\ Libraries/clock.d ./THT_Libraries/Middle\ Libraries/clock.o ./THT_Libraries/Middle\ Libraries/dev_config.d ./THT_Libraries/Middle\ Libraries/dev_config.o ./THT_Libraries/Middle\ Libraries/fota.d ./THT_Libraries/Middle\ Libraries/fota.o ./THT_Libraries/Middle\ Libraries/http.d ./THT_Libraries/Middle\ Libraries/http.o ./THT_Libraries/Middle\ Libraries/level_sensor.d ./THT_Libraries/Middle\ Libraries/level_sensor.o ./THT_Libraries/Middle\ Libraries/power.d ./THT_Libraries/Middle\ Libraries/power.o ./THT_Libraries/Middle\ Libraries/sdcard.d ./THT_Libraries/Middle\ Libraries/sdcard.o ./THT_Libraries/Middle\ Libraries/sim.d ./THT_Libraries/Middle\ Libraries/sim.o ./THT_Libraries/Middle\ Libraries/sms.d ./THT_Libraries/Middle\ Libraries/sms.o ./THT_Libraries/Middle\ Libraries/telegram.d ./THT_Libraries/Middle\ Libraries/telegram.o ./THT_Libraries/Middle\ Libraries/wlv_def.d ./THT_Libraries/Middle\ Libraries/wlv_def.o

.PHONY: clean-THT_Libraries-2f-Middle-20-Libraries
