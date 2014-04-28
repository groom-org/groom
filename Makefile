AVRGCC     = avr-gcc
CFLAGS     = -Iinclude -Os -Wall -std=c99

DEVICE     = atmega328p
CLOCK      = 9830400
UBRR       = 63
FUSES      = -U hfuse:w:0xd9:m -U lfuse:w:0xe0:m
PROGRAMMER = -c avrispmkii -P usb

CFLAGS    += -DF_CPU=$(CLOCK)
CFLAGS    += -DF_UBRR=$(UBRR)
CFLAGS    += -mmcu=$(DEVICE)

CFLAGS_MASTER = -DGROOM_MASTER

CFLAGS_ALPHA  = -DGROOM_ALPHA

CFLAGS_BETA   = -DGROOM_BETA

AVRDUDE    = avrdude $(PROGRAMMER) -p $(DEVICE)

OBJECTS_MASTER    = groom_master.o
OBJECTS_MASTER   += spi_master.o
OBJECTS_MASTER   += usart_master.o
OBJECTS_MASTER   += encoder_master.o
OBJECTS_MASTER   += tft_master.o
OBJECTS_MASTER   += button_master.o
OBJECTS_MASTER   += usart_mux_master.o
OBJECTS_MASTER   += i2c_master.o
OBJECTS_MASTER   += rtc_master.o

OBJECTS_ALPHA     = groom_alpha.o
OBJECTS_ALPHA    += usart_alpha.o

OBJECTS_BETA      = groom_beta.o
OBJECTS_BETA     += usart_beta.o

all: groom_master.hex groom_alpha.hex groom_beta.hex

groom_master.o: groom_master.c
	$(AVRGCC) -c $(CFLAGS) $(CFLAGS_MASTER) $< -o $@

groom_alpha.o: groom_alpha.c
	$(AVRGCC) -c $(CFLAGS) $(CFLAGS_ALPHA) $< -o $@

groom_beta.o: groom_beta.c
	$(AVRGCC) -c $(CFLAGS) $(CFLAGS_BETA) $< -o $@

%.o: %.c
	$(AVRGCC) -c $(CFLAGS) $< -o $@

%_master.o: %.c
	$(AVRGCC) -c $(CFLAGS) $(CFLAGS_MASTER) $< -o $@

%_alpha.o: %.c
	$(AVRGCC) -c $(CFLAGS) $(CFLAGS_ALPHA) $< -o $@

%_beta.o: %.c
	$(AVRGCC) -c $(CFLAGS) $(CFLAGS_BETA) $< -o $@

flash_master: groom_master.hex
	$(AVRDUDE) -U flash:w:groom_master.hex:i

flash_alpha: groom_alpha.hex
	$(AVRDUDE) -U flash:w:groom_alpha.hex:i

flash_beta: groom_beta.hex
	$(AVRDUDE) -U flash:w:groom_beta.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

install: flash fuse

groom_master.elf: $(OBJECTS_MASTER)
	$(AVRGCC) $(CFLAGS) $(CFLAGS_MASTER) $(OBJECTS_MASTER) -o groom_master.elf

groom_alpha.elf: $(OBJECTS_ALPHA)
	$(AVRGCC) $(CFLAGS) $(CFLAGS_ALPHA) $(OBJECTS_ALPHA) -o groom_alpha.elf

groom_beta.elf: $(OBJECTS_BETA)
	$(AVRGCC) $(CFLAGS) $(CFLAGS_BETA) $(OBJECTS_BETA) -o groom_beta.elf

groom_master.hex: groom_master.elf
	-rm -f groom_master.hex
	avr-objcopy -j .text -j .data -O ihex groom_master.elf groom_master.hex

groom_alpha.hex: groom_alpha.elf
	-rm -f groom_alpha.hex
	avr-objcopy -j .text -j .data -O ihex groom_alpha.elf groom_alpha.hex

groom_beta.hex: groom_beta.elf
	-rm -f groom_beta.hex
	avr-objcopy -j .text -j .data -O ihex groom_beta.elf groom_beta.hex

disasm_master: groom_master.elf
	avr-objdump -d groom_master.elf

disasm_alpha: groom_alpha.elf
	avr-objdump -d groom_alpha.elf
	
disasm_beta: groom_beta.elf
	avr-objdump -d groom_beta.elf

clean:
	-rm -f *.o
	-rm -f *.elf
	-rm -f *.hex
