CC = arm-eabi-gcc
OBJCOPY = arm-eabi-objcopy
FLAGS = -mthumb -mcpu=cortex-m4
CPPFLAGS = -DSTM32F411xE
CFLAGS = $(FLAGS) -Wall -g \
				 -O0 -ffunction-sections -fdata-sections \
				 -I/opt/arm/stm32/inc \
				 -I/opt/arm/stm32/CMSIS/Include \
				 -I/opt/arm/stm32/CMSIS/Device/ST/STM32F4xx/Include
LDFLAGS = $(FLAGS) -Wl,--gc-sections -nostartfiles \
					-L/opt/arm/stm32/lds -Tstm32f411re.lds
vpath %.c /opt/arm/stm32/src

OBJECTS = \
	main.o \
	led.o \
	startup_stm32.o \
	delay.o \
	gpio.o \
	lcd_util.o \
	fonts.o \
	sbrk.o \
	accelerometer.o \
	utility.o \
	button.o

TARGET = main
.SECONDARY: $(TARGET).elf $(OBJECTS)
all: $(TARGET).bin
%.elf : $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@
%.bin : %.elf
	$(OBJCOPY) $< $@ -O binary
deploy: $(TARGET).bin
	/opt/arm/stm32/ocd/qfn4 
clean:
	rm -f *.bin *.elf *.hex *.d *.o *.bak *~
