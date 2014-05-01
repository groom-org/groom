uint8_t logo_width = 32;
uint8_t logo_height = 32;

#ifdef __AVR__
 #include <avr/io.h>
 #include <avr/pgmspace.h>
#else
 #define PROGMEM
#endif

static const uint16_t logo_data[] = {
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0008,
	0x0055,
	0x0055,
	0x0055,
	0x0045,
	0x0012,
	0x0012,
	0x0008,
	0x0008,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x034C,
	0x30D9,
	0x08A3,
	0x039F,
	0x1455,
	0x1D7F,
	0x1D7F,
	0x1D7F,
	0x1C76,
	0x1B76,
	0x1B76,
	0x1C7E,
	0x035B,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0001,
	0x0029,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0252,
	0x0252,
	0x3C87,
	0x7D18,
	0x3E8F,
	0x7D28,
	0x1B7F,
	0x0008,
	0x0034,
	0x0034,
	0x031B,
	0x0212,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0026,
	0x0044,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0296,
	0x7D18,
	0x3E87,
	0x7D28,
	0x035F,
	0x0045,
	0x3C87,
	0x3C87,
	0x7D28,
	0x7D28,
	0x124C,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0026,
	0x0044,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0296,
	0x7D18,
	0x3E87,
	0x7D28,
	0x035F,
	0x0045,
	0x3C87,
	0x3C87,
	0x7D28,
	0x7D28,
	0x124C,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0002,
	0x00B6,
	0x1937,
	0x3E87,
	0x1C76,
	0x07A3,
	0x0008,
	0x0008,
	0x0000,
	0x1B76,
	0x7D18,
	0x3E8F,
	0x0067,
	0x1665,
	0x7D28,
	0x7D28,
	0x3E87,
	0x3E8F,
	0x7D28,
	0x0212,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0090,
	0x011F,
	0x00FB,
	0x36FA,
	0x3E97,
	0x7D28,
	0x186D,
	0x186D,
	0x0000,
	0x1665,
	0x7D39,
	0x3A87,
	0x0067,
	0x3A87,
	0x3E8F,
	0x3E8F,
	0x3E87,
	0x3E87,
	0x7D28,
	0x1254,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0001,
	0x0001,
	0x0001,
	0x00B3,
	0x011D,
	0x00DC,
	0x26DF,
	0x3E8E,
	0x3E87,
	0x7D28,
	0x7D28,
	0x031B,
	0x0077,
	0x035F,
	0x124C,
	0x0296,
	0x7D18,
	0x3E87,
	0x3E87,
	0x3E87,
	0x3E87,
	0x7D18,
	0x1765,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0017,
	0x0028,
	0x0028,
	0x000E,
	0x00FD,
	0x00FE,
	0x2955,
	0x3E8E,
	0x3E87,
	0x7D28,
	0x7D28,
	0x035F,
	0x0000,
	0x0000,
	0x0000,
	0x0256,
	0x7D28,
	0x3E87,
	0x3E87,
	0x3E87,
	0x3E87,
	0x7D18,
	0x165D,
	0x0055,
	0x0067,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x00B3,
	0x003C,
	0x003C,
	0x0000,
	0x0017,
	0x00B7,
	0x091B,
	0x751A,
	0x3E97,
	0x1A76,
	0x1A76,
	0x000A,
	0x0012,
	0x0008,
	0x0000,
	0x0008,
	0x1B76,
	0x7D18,
	0x7D18,
	0x3E87,
	0x3E87,
	0x7D28,
	0x104C,
	0x035F,
	0x114C,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0007,
	0x00FD,
	0x002A,
	0x002A,
	0x0000,
	0x0000,
	0x0000,
	0x000E,
	0x04AC,
	0x02D6,
	0x0000,
	0x0000,
	0x035F,
	0x7D18,
	0x1454,
	0x0000,
	0x0000,
	0x0077,
	0x3C87,
	0x3C87,
	0x7D28,
	0x7D28,
	0x7D18,
	0x0057,
	0x1765,
	0x1B76,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x004E,
	0x011F,
	0x0029,
	0x0029,
	0x0029,
	0x0002,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x02DA,
	0x02DA,
	0x7D18,
	0x7D18,
	0x7D28,
	0x039F,
	0x0000,
	0x0000,
	0x0077,
	0x0077,
	0x1765,
	0x186E,
	0x0067,
	0x0212,
	0x7D18,
	0x3E8F,
	0x0067,
	0x0067,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x004E,
	0x011F,
	0x0029,
	0x0029,
	0x0029,
	0x0002,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x02DA,
	0x02DA,
	0x7D18,
	0x7D18,
	0x7D28,
	0x039F,
	0x0000,
	0x0000,
	0x0077,
	0x0077,
	0x1765,
	0x186E,
	0x0067,
	0x0212,
	0x7D18,
	0x3E8F,
	0x0067,
	0x0067,
	0x0000,
	0x0000,
	0x0000,
	0x0002,
	0x00D9,
	0x011E,
	0x0028,
	0x0028,
	0x00B3,
	0x00D9,
	0x002B,
	0x000F,
	0x0017,
	0x1355,
	0x3E8F,
	0x3E8F,
	0x3E8F,
	0x3E87,
	0x3E8F,
	0x7D18,
	0x031A,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0067,
	0x3A87,
	0x3E8F,
	0x7D28,
	0x0F44,
	0x0F44,
	0x0000,
	0x0000,
	0x0000,
	0x003C,
	0x00FD,
	0x00DC,
	0x0070,
	0x0070,
	0x00D9,
	0x00DD,
	0x00DD,
	0x00DD,
	0x00DD,
	0x36FB,
	0x3E8E,
	0x3E8E,
	0x3E87,
	0x3E87,
	0x3E87,
	0x3E8F,
	0x7D18,
	0x155D,
	0x031B,
	0x031B,
	0x02DA,
	0x1454,
	0x3E8F,
	0x7D18,
	0x3E87,
	0x7D18,
	0x196E,
	0x196E,
	0x0000,
	0x0000,
	0x0000,
	0x02FF,
	0x1359,
	0x0B17,
	0x0AF7,
	0x0AF7,
	0x0AF7,
	0x0AF7,
	0x0AF7,
	0x0AD7,
	0x02D9,
	0x493E,
	0x3E86,
	0x3E86,
	0x3E87,
	0x3E87,
	0x3E87,
	0x3E87,
	0x3E87,
	0x7D18,
	0x7D28,
	0x7D28,
	0x7D28,
	0x7D28,
	0x3E8F,
	0x3E87,
	0x3E87,
	0x3E87,
	0x3E8F,
	0x3E8F,
	0x0067,
	0x0000,
	0x0000,
	0x0057,
	0x13F7,
	0x13D5,
	0x13D5,
	0x13D5,
	0x13D5,
	0x13D5,
	0x13D5,
	0x13D5,
	0x03F6,
	0x5351,
	0x3E86,
	0x3E86,
	0x3E8E,
	0x3E8E,
	0x3E8E,
	0x3E8E,
	0x3E8E,
	0x3E8E,
	0x3E8E,
	0x3E8E,
	0x3E8F,
	0x7D18,
	0x7D18,
	0x7D18,
	0x7D18,
	0x7D18,
	0x7D38,
	0x7D38,
	0x114C,
	0x0000,
	0x0000,
	0x0005,
	0x1374,
	0x13B6,
	0x13B6,
	0x13B6,
	0x13B6,
	0x13B6,
	0x13B6,
	0x13B6,
	0x0BD6,
	0x3355,
	0x6592,
	0x6592,
	0x6592,
	0x6592,
	0x6592,
	0x6592,
	0x6592,
	0x6592,
	0x6993,
	0x6993,
	0x5D70,
	0x0256,
	0x0256,
	0x0256,
	0x0256,
	0x0256,
	0x0296,
	0x0296,
	0x00E9,
	0x0000,
	0x0000,
	0x0000,
	0x053F,
	0x13F7,
	0x13B6,
	0x13B6,
	0x13B6,
	0x13B6,
	0x13B6,
	0x13B6,
	0x03D5,
	0x7A37,
	0x5D18,
	0x5D18,
	0x5958,
	0x5958,
	0x5958,
	0x5958,
	0x5958,
	0x5958,
	0x617A,
	0x617A,
	0x0A4B,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0057,
	0x13F7,
	0x13B6,
	0x13B6,
	0x13B6,
	0x13B6,
	0x13B6,
	0x0BD5,
	0x2B56,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5D78,
	0x4D33,
	0x4D33,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0057,
	0x13F7,
	0x13B6,
	0x13B6,
	0x13B6,
	0x13B6,
	0x13B6,
	0x0BD5,
	0x2B56,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5D78,
	0x4D33,
	0x4D33,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x000A,
	0x1374,
	0x13B6,
	0x13B6,
	0x13B6,
	0x13B6,
	0x13B6,
	0x03D5,
	0x7A37,
	0x6137,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5D78,
	0x4912,
	0x4912,
	0x0A5B,
	0x0B5C,
	0x0B5B,
	0x0A5B,
	0x0A4A,
	0x0B5B,
	0x019E,
	0x019E,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x054F,
	0x13F7,
	0x13F7,
	0x13B6,
	0x13B6,
	0x0BD5,
	0x2B56,
	0x5D57,
	0x5D57,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5D78,
	0x5D78,
	0x6179,
	0x6179,
	0x6179,
	0x6179,
	0x6579,
	0x657A,
	0x0196,
	0x0196,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x02B8,
	0x13F7,
	0x13F7,
	0x13B6,
	0x13B6,
	0x03D5,
	0x7A37,
	0x6137,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x6579,
	0x0D6D,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x000E,
	0x1395,
	0x1395,
	0x13B6,
	0x0BD5,
	0x2B56,
	0x5D57,
	0x5D57,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x6178,
	0x5135,
	0x0009,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x054F,
	0x054F,
	0x13F7,
	0x03D5,
	0x7A37,
	0x6137,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5D57,
	0x6179,
	0x03B8,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x02B8,
	0x02B8,
	0x0BF7,
	0x2B56,
	0x5D57,
	0x5D57,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x5957,
	0x6179,
	0x20F0,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x000E,
	0x000E,
	0x03B5,
	0x7A57,
	0x6137,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D78,
	0x5957,
	0x0037,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x000E,
	0x000E,
	0x03B5,
	0x7A57,
	0x6137,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D57,
	0x5D78,
	0x5957,
	0x0037,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x3230,
	0x619A,
	0x6158,
	0x6178,
	0x6178,
	0x5D78,
	0x5D78,
	0x5D78,
	0x5D78,
	0x5D78,
	0x5D78,
	0x5D78,
	0x5D78,
	0x5D78,
	0x5D78,
	0x5D78,
	0x5D78,
	0x6579,
	0x0A4A,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0037,
	0x0196,
	0x0156,
	0x0156,
	0x0156,
	0x0155,
	0x0155,
	0x0155,
	0x0155,
	0x0155,
	0x0155,
	0x0155,
	0x0155,
	0x0155,
	0x0155,
	0x0155,
	0x0155,
	0x0155,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000
};