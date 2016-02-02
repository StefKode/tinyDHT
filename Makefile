########################################################################################
#
# Copyright by Stefan Koch <StefanKoch@gmx.org>, 2016
#
# ATTiny85 Makefile
# This file is part of TinyAvrMakefileProject
#
#    omxAutomation is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    TinyAvrMakefileProject is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with TinyAvrMakefileProject. If not, see <http://www.gnu.org/licenses/>.
#
########################################################################################

#adapt the name to your needs
NAME = hello_dht

#-------------------DEVICE-----------------------------------------------------------
DEVICE        = attiny85
COREFREQ      = 8000000

#-------------------BINUTILS---------------------------------------------------------
CROSS_COMPILE = avr-
CROSS_CC      = $(CROSS_COMPILE)gcc
CROSS_LD      = $(CROSS_COMPILE)gcc
CROSS_OBJCOPY = $(CROSS_COMPILE)objcopy
CROSS_SIZE    = $(CROSS_COMPILE)size

# HEXIFY requires the .o and .hex filename to be appended
HEXIFY        = $(CROSS_OBJCOPY) -j .text -j .data -O ihex

#-------------------TOOLCHAIN_CONFIG-------------------------------------------------
CROSS_CFLAGS  = -Wall -gdwarf-2 -Os -std=gnu99 -mmcu=$(DEVICE) -DF_CPU=$(COREFREQ) 
CROSS_CFLAGS += -fno-inline-small-functions -ffunction-sections -fdata-sections 
CROSS_CFLAGS += -Wl,--relax,--gc-sections -I.
CROSS_CFLAGS += -Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000 
CROSS_LDFLAGS = -g -mmcu=$(DEVICE)

#-------------------GENERIC_DEFS-----------------------------------------------------
SOURCES    = $(wildcard *.c)
SOURCESASM = $(wildcard *.S)
HEADERS    = $(wildcard *.h)
OBJECTS    = $(SOURCES:.c=.o)
OBJECTSASM = $(SOURCES:.S=.o)

#-------------------INTRO_RULES------------------------------------------------------
all:			$(NAME).hex $(HEADERS)

$(NAME).hex:	$(OBJECTS) $(OBJECTSASM)
				@echo [LD] link
				@$(CROSS_CC) $(CROSS_CFLAGS) $(OBJECTS) -o $(NAME).elf
				@$(CROSS_SIZE) $(NAME).elf | grep $(NAME).elf | awk '{print "[Si] Program_Size = "$$1" bytes"}'
				@echo [HX] generate hex file
				@$(HEXIFY) $(NAME).elf $(NAME).hex

#-------------------GENERIC_RULES----------------------------------------------------
%.o:			%.c
				@echo [CC] $@
				@$(CROSS_CC) -c $(CROSS_CFLAGS) ${^} -o ${@}

#-------------------SPECIFIC_RULES---------------------------------------------------
prog:			$(NAME).hex
				@echo [PR] program flash
				@avrdude -c usbasp -p t85 -e -U flash:w:$(NAME).hex

clean:
				@echo [CL] clean
				@rm -f *.o *.o *.hex *.elf 

distclean:		clean

setuc8mhz:
				avrdude -c usbasp -p t85 -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m 
