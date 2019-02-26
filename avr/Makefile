# the GNU make program is found at $(ARDUINO_DIR)\hardware\tools\avr\utils\bin
#

VERSION      := 0.1
PROGRAM_NAME := junetest$(VERSION)


# Chip and programmer 
DEVICE_PROGRAMMER := usbtiny
CPU_AVRDUDE := m328p

MCU		:= atmega328p
F_CPU       := 	1200000LL


LIB_PATH	:= $(JUNE_ROOT)/avr
LIBS		:= -lJune -lm
INCLUDES	:= -I$(JUNE_ROOT)/avr

# FLAGS
CF      := -c -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(INCLUDES)
LF      := -Os -MD -MP -mmcu=$(MCU) -MT -static -L$(LIB_PATH) $(LIBS)
# TOOLS
CC          := avr-g++
avrdude     ?= avrdude


OBJECTS = main.o


all: $(PROGRAM_NAME)

main.o: main.cpp
	$(CC) $(CF) -c $<

$(PROGRAM_NAME): $(OBJECTS)
	$(MAKE) -C $(JUNE_ROOT)/avr
	$(CC) -o $(PROGRAM_NAME) $(OBJECTS) $(LF)
	

$(PROGRAM_NAME).hex: $(PROGRAM_NAME)
	avr-objcopy -O ihex -R .eeprom $(PROGRAM_NAME) $(PROGRAM_NAME).hex

.PHONY: flash
flash: programmedDevice


clean:
	-rm -f *.o *.hex *.elf programmedDevice_*.time

programmedDevice:  $(PROGRAM_NAME).hex
	$(avrdude) -c $(DEVICE_PROGRAMMER) -p $(CPU_AVRDUDE) -v -U flash:w:$(PROGRAM_NAME).hex
	touch programmedDevice_$(CPU_GCC).time

$(PROGRAM_NAME)_$(CPU_GCC).hex: $(Ofiles)
	$(CC) $(LF) -o $(PROGRAM_NAME)_$(CPU_GCC).elf $(Ofiles)
	$(avr_objcopy) -j .text -j .data -O ihex  $(PROGRAM_NAME)_$(CPU_GCC).elf  $(PROGRAM_NAME)_$(CPU_GCC).hex

clean:
	-rm -f *.o *.hex *.elf programmedDevice_*.time
	$(MAKE) -C $(JUNE_ROOT)/avr  clean

dumpchip_srec: 
	$(avrdude) -c $(DEVICE_PROGRAMMER) -p $(CPU_AVRDUDE) -U lfuse:r:lfuse_$(CPU_GCC).hex:i -U hfuse:r:hfuse_$(CPU_GCC).hex:i -U efuse:r:efuse_$(CPU_GCC).hex:i -U eeprom:r:eeprom_$(CPU_GCC).hex:i -U flash:r:flash_$(CPU_GCC).hex:i -U lock:r:lock_$(CPU_GCC).hex:i -U signature:r:sig_$(CPU_GCC).hex:i 

dumpchip: 
	$(avrdude) -c $(DEVICE_PROGRAMMER) -p $(CPU_AVRDUDE) -U lfuse:r:lfuse_$(CPU_GCC).bin:r -U hfuse:r:hfuse_$(CPU_GCC).bin:r -U eeprom:r:eeprom_$(CPU_GCC).bin:r -U flash:r:flash_$(CPU_GCC).bin:r -U lock:r:lock_$(CPU_GCC).bin:r -U signature:r:sig_$(CPU_GCC).bin:r -U efuse:r:efuse_$(CPU_GCC).bin:r 

check:
	$(avrdude) -v -c $(DEVICE_PROGRAMMER) -p $(CPU_AVRDUDE)

