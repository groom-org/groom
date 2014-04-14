AVRGCC     = avr-gcc
CFLAGS     = -Iinclude -Os -Wall

DEVICE     = atmega328p
CLOCK      = 9830400
UBRR       = 63
FUSES      = -U hfuse:w:0xd9:m -U lfuse:w:0xe0:m
PROGRAMMER = -c avrispmkii -P usb

CFLAGS    += -DF_CPU=$(CLOCK)
CFLAGS    += -DF_UBRR=$(UBRR)
CFLAGS    += -mmcu=$(DEVICE)

AVRDUDE    = avrdude $(PROGRAMMER) -p $(DEVICE)

OBJECTS    = groom.o
OBJECTS   += spi.o
OBJECTS   += usart.o
OBJECTS   += encoder.o
OBJECTS   += tft.o

all: groom.hex

.c.o:
	$(AVRGCC) -c $(CFLAGS) $< -o $@

flash: all
	$(AVRDUDE) -U flash:w:groom.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

install: flash fuse

groom.elf: $(OBJECTS)
	$(AVRGCC) $(CFLAGS) $(OBJECTS) -o groom.elf

groom.hex: groom.elf
	-rm -f groom.hex
	avr-objcopy -j .text -j .data -O ihex groom.elf groom.hex

disasm: groom.elf
	avr-objdump -d groom.elf

clean:
	-rm -f *.o
	-rm -f *.elf
	-rm -f *.hex
