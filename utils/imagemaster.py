# Read image and store values into all_lines

from PIL import Image
im = Image.open("gtestmaster.png")
pix = im.load()
all_lines = []

print im.size
for i in range (0,32):
	for j in range (0,32):
		all_lines.append(str(pix[i,j]))

#-----------

# Read all_lines and convert values into 0x0000 hex. outputs to file

import sys
fo = open("final.txt", "wb")
red = []
green = []
blue = []
a = []
xpos = 0
ypos = 0

def dtb(x):
	return int(bin(x)[2:])

for i in range(0,len(all_lines)):
	if len(all_lines[i].split()) > 0:
		red.append((int(all_lines[i].split()[0][1:-1]))/8)
		green.append((int(all_lines[i].split()[1][:-1]))/4)
		blue.append((int(all_lines[i].split()[2][:-1]))/8)
		a.append(int(all_lines[i].split()[3][:-1])/8)

for i in range(0,32):
	for j in range(0,32):
		temp =  hex(int(str(dtb(red[i*32 + j])) + str(dtb(green[i*32 +j]))+ str(dtb(blue[i*32 +j])),2))
		#print temp
		#fo.write( "tft_draw_pixel(0x"+'{:0>4}'.format(str(hex(xpos+i)[2:]).upper()) + ", 0x" +'{:0>4}'.format(str(hex(ypos+j)[2:]).upper())+", 0x"+ '{:0>4}'.format(temp.upper()[2:])+");" )
		fo.write( "0x"+ '{:0>4}'.format(temp.upper()[2:])+"," )
		fo.write("\n")
