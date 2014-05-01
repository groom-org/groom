AVRGCC     = avr-gcc
CFLAGS     = -Iinclude -Os -Wall -std=c99

CC         = $(AVRGCC)
LD         = $(CC)

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
OBJECTS_MASTER   += com_master.o
OBJECTS_MASTER   += usart_mux_master.o

OBJECTS_ALPHA     = groom_alpha.o
OBJECTS_ALPHA    += usart_alpha.o

OBJECTS_BETA      = groom_beta.o
OBJECTS_BETA     += usart_beta.o

all: groom_master.hex groom_alpha.hex groom_beta.hex

groom_master.o: groom_master.c
	$(CC) -c $(CFLAGS) $(CFLAGS_MASTER) $< -o $@

groom_alpha.o: groom_alpha.c
	$(CC) -c $(CFLAGS) $(CFLAGS_ALPHA) $< -o $@

groom_beta.o: groom_beta.c
	$(CC) -c $(CFLAGS) $(CFLAGS_BETA) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%_master.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGS_MASTER) $< -o $@

%_alpha.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGS_ALPHA) $< -o $@

%_beta.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGS_BETA) $< -o $@

flash_%: groom_%.hex
	$(AVRDUDE) -U flash:w:$<:i
fuse:
	$(AVRDUDE) $(FUSES)

install: flash fuse

groom_master.elf: $(OBJECTS_MASTER)
	$(LD) $(CFLAGS) $(CFLAGS_MASTER) $^ -o $@

groom_alpha.elf: $(OBJECTS_ALPHA)
	$(LD) $(CFLAGS) $(CFLAGS_ALPHA) $^ -o $@

groom_beta.elf: $(OBJECTS_BETA)
	$(LD) $(CFLAGS) $(CFLAGS_BETA) $^ -o $@

%.hex: %.elf
	-rm -f $@
	avr-objcopy -j .text -j .data -O ihex $< $@

disasm_%: groom_%.elf
	avr-objdump -d $<
clean:
	-rm -f *.o
	-rm -f *.elf
	-rm -f *.hex
