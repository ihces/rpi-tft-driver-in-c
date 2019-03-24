#include <wiringPi.h>
#include <stdint.h>
#include "ili9486.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define swap(type, i, j) {type t = i; i = j; j = t;}
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))

int disp_x_size = 319;
int disp_y_size = 479;

typedef enum{
    CS = 7,  // 7
    RST = 0, // 11
    RS = 2,  // 13
    WR = 3   // 15
} Pins;

// physical pin nums: 12, 16, 18, 22, 24, 26, 32, 36, 38, 40, 37, 35, 33, 31, 29, 23
int dataBus[] = {1, 4, 5, 6, 10, 11, 26, 27, 28, 29, 25, 24, 23, 22, 21, 14};

Orientation orient = LANDSCAPE;

void pulse_low(int pin) {
	digitalWrite(pin, 0);
	digitalWrite(pin, 1);
}

void _write2bus(uint16_t data) {
	int i;
        uint16_t mask = 1;
        for (i = 0; i < 16; ++i) {
		digitalWrite(dataBus[i], data & mask);
		mask = mask << 1;
	}
//	delay(15);
}

void write2bus(uint16_t data) {
	_write2bus(data);
	pulse_low(WR);
}

void sendCommand(uint16_t cmd) {
    digitalWrite(RS, 0);
    write2bus(cmd);
}

void sendData(uint16_t data) {
    digitalWrite(RS, 1);
    write2bus(data);
}

void initScreen (Orientation orientation) {
    orient=orientation;

    int i;

    wiringPiSetup();

    pinMode(CS, OUTPUT);
    pinMode(RST, OUTPUT);
    pinMode(RS, OUTPUT);
    pinMode(WR, OUTPUT);

    // DataBus Setup
    for (i = 0 ; i < 16 ; ++i)
      pinMode(dataBus[i], OUTPUT);

	digitalWrite(RST, 1);
	delay(5);
	digitalWrite(RST, 0);
	delay(15);
	digitalWrite(RST, 1);
	delay(15);

	digitalWrite(CS, 0);

	// ILI9486
	sendCommand(0x11);		// Sleep OUT
	delay(50);

	sendCommand(0xF2);		// ?????
	sendData(0x1C);
	sendData(0xA3);
	sendData(0x32);
	sendData(0x02);
	sendData(0xb2);
	sendData(0x12);
	sendData(0xFF);
	sendData(0x12);
	sendData(0x00);

	sendCommand(0xF1);		// ?????
	sendData(0x36);
	sendData(0xA4);

	sendCommand(0xF8);		// ?????
	sendData(0x21);
	sendData(0x04);

	sendCommand(0xF9);		// ?????
	sendData(0x00);
	sendData(0x08);

	sendCommand(0xC0);		// Power Control 1
	sendData(0x0d);
	sendData(0x0d);

	sendCommand(0xC1);		// Power Control 2
	sendData(0x43);
	sendData(0x00);

	sendCommand(0xC2);		// Power Control 3
	sendData(0x00);

	sendCommand(0xC5);		// VCOM Control
	sendData(0x00);
	sendData(0x48);

	sendCommand(0xB6);		// Display Function Control
	sendData(0x00);
	sendData(0x22);		// 0x42 = Rotate display 180 deg.
	sendData(0x3B);

	sendCommand(0xE0);		// PGAMCTRL (Positive Gamma Control)
	sendData(0x0f);
	sendData(0x24);
	sendData(0x1c);
	sendData(0x0a);
	sendData(0x0f);
	sendData(0x08);
	sendData(0x43);
	sendData(0x88);
	sendData(0x32);
	sendData(0x0f);
	sendData(0x10);
	sendData(0x06);
	sendData(0x0f);
	sendData(0x07);
	sendData(0x00);

	sendCommand(0xE1);		// NGAMCTRL (Negative Gamma Control)
	sendData(0x0F);
	sendData(0x38);
	sendData(0x30);
	sendData(0x09);
	sendData(0x0f);
	sendData(0x0f);
	sendData(0x4e);
	sendData(0x77);
	sendData(0x3c);
	sendData(0x07);
	sendData(0x10);
	sendData(0x05);
	sendData(0x23);
	sendData(0x1b);
	sendData(0x00);

	sendCommand(0x20);		// Display Inversion OFF
	sendData(0x00);//C8

	sendCommand(0x36);		// Memory Access Control
	sendData(0x0A);

	sendCommand(0x3A);		// Interface Pixel Format
	sendData(0x55);

	sendCommand(0x2A);		// Column Addess Set
	sendData(0x00);
	sendData(0x00);
	sendData(0x01);
	sendData(0xDF);

	sendCommand(0x002B);		// Page Address Set
	sendData(0x00);
	sendData(0x00);
	sendData(0x01);
	sendData(0x3f);
	delay(50);
	sendCommand(0x0029);		// Display ON
	sendCommand(0x002C);		// Memory Write
	// ILI9486

	digitalWrite(CS, 0);
}

void setArea(Point p1, Point p2) {
    if (orient==LANDSCAPE)
	{
		swap(uint16_t, p1.x, p1.y);
		swap(uint16_t, p2.x, p2.y);
		p1.y = disp_y_size-p1.y;
		p2.y = disp_y_size-p2.y;
		swap(uint16_t, p1.y, p2.y);
	}
	sendCommand(0x2a);
	sendData(p1.x>>8);
	sendData(p1.x);
	sendData(p2.x>>8);
	sendData(p2.x);
	sendCommand(0x2b);
	sendData(p1.y>>8);
	sendData(p1.y);
	sendData(p2.y>>8);
	sendData(p2.y);
	sendCommand(0x2c);
}

void resetArea() {
	if (orient == PORTRAIT)
		setArea(Point(0, 0), Point(disp_x_size, disp_y_size));
	else
		setArea(Point(0, 0), Point(disp_y_size, disp_x_size));
}

uint16_t getColor16b(RGB color)
{
	return (((color.R & 248) | color.G >> 5)<<8) | ((color.G & 28)<<3 | color.B >> 3);
}

void fastFill(RGB color, int numOfPixel) {
        _write2bus(getColor16b(color));

	long blocks = numOfPixel / 16;
	for (int i=0; i < blocks; i++)
	{
		pulse_low(WR);
		pulse_low(WR);
		pulse_low(WR);
		pulse_low(WR);
		pulse_low(WR);
		pulse_low(WR);
		pulse_low(WR);
		pulse_low(WR);
		pulse_low(WR);
		pulse_low(WR);
		pulse_low(WR);
		pulse_low(WR);
		pulse_low(WR);
		pulse_low(WR);
		pulse_low(WR);
		pulse_low(WR);
	}
	if ((numOfPixel % 16) != 0)
		for (int i=0; i<(numOfPixel % 16)+1; i++)
		    pulse_low(WR);
}

void drawPixel(Point p, RGB color) {
	digitalWrite(CS, 0);
	setArea(p, p);
	sendData(getColor16b(color));
	digitalWrite(CS, 1);
	resetArea();
}

void drawHLine(Point p, int l, RGB color) {
	if (l<0)
	{
		l = -l;
		p.x -= l;
	}

	digitalWrite(CS, 0);
	setArea(p, Point(p.x+l, p.y));
	digitalWrite(RS, 1);
	fastFill(color,l);
	digitalWrite(CS, 1);
	resetArea();
}

void drawVLine(Point p, int l, RGB color) {
	if (l<0) {
		l = -l;
		p.y -= l;
	}

	digitalWrite(CS, 0);
	setArea(p, Point(p.x, p.y+l));
	digitalWrite(RS, 1);
	fastFill(color,l);
	digitalWrite(CS, 1);
	resetArea();
}

void drawLine(Point p1, Point p2, RGB color) {
	if (p1.y == p2.y)
		drawHLine(p1, p2.x - p1.x, color);
	else if (p1.x == p2.x)
		drawVLine(p1, p2.y - p1.y, color);
	else
	{
		unsigned int	dx = (p2.x > p1.x ? p2.x - p1.x : p1.x - p2.x);
		short			xstep =  p2.x > p1.x ? 1 : -1;
		unsigned int	dy = (p2.y > p1.y ? p2.y - p1.y : p1.y - p2.y);
		short			ystep =  p2.y > p1.y ? 1 : -1;
		int				col = p1.x, row = p1.y;

		digitalWrite(CS, 0);
		if (dx < dy)
		{
			int t = - (dy >> 1);
			while (true)
			{
				setArea(Point(col, row), Point(col, row));
				sendData(getColor16b(color));
				if (row == p2.y)
					return;
				row += ystep;
				t += dx;
				if (t >= 0)
				{
					col += xstep;
					t   -= dy;
				}
			}
		}
		else
		{
			int t = - (dx >> 1);
			while (true)
			{
				setArea(Point(col, row), Point(col, row));
				sendData(getColor16b(color));
				if (col == p2.x)
					return;
				col += xstep;
				t += dy;
				if (t >= 0)
				{
					row += ystep;
					t   -= dx;
				}
			}
		}
		digitalWrite(CS, 1);
	}
	resetArea();
}

void drawRect(Point p1, Point p2, RGB color) {
	if (p1.x > p2.x)
		swap(int, p1.x, p2.x);

	if (p1.y > p2.y)
		swap(int, p1.y, p2.y);

	drawHLine(p1, p2.x - p1.x, color);
	drawHLine(Point(p1.x, p2.y), p2.x - p1.x, color);
	drawVLine(p1, p2.y - p1.y, color);
	drawVLine(Point(p2.x, p1.y), p2.y - p1.y, color);
}

void drawRoundRect(Point p1, Point p2, RGB color) {
	if (p1.x > p2.x)
		swap(int, p1.x, p2.x);
	if (p1.y > p2.y)
		swap(int, p1.y, p2.y);
	if ((p2.x - p1.x) > 4 && (p2.y - p1.y) > 4)
	{
		drawPixel(Point(p1.x + 1, p1.y + 1), color);
		drawPixel(Point(p2.x - 1, p1.y + 1), color);
		drawPixel(Point(p1.x + 1, p2.y - 1), color);
		drawPixel(Point(p2.x - 1, p2.y - 1), color);
		drawHLine(Point(p1.x + 2, p1.y), p2.x - p1.x - 4, color);
		drawHLine(Point(p1.x + 2, p2.y), p2.x - p1.x - 4, color);
		drawVLine(Point(p1.x, p1.y + 2), p2.y - p1.y - 4, color);
		drawVLine(Point(p2.x, p1.y + 2), p2.y - p1.y - 4, color);
	}
}

void fillRect(Point p1, Point p2, RGB color) {
	if (p1.x > p2.x)
		swap(int, p1.x, p2.x);

	if (p1.y > p2.y)
		swap(int, p1.y, p2.y);

	digitalWrite(CS, 0);
	setArea(p1, p2);
	digitalWrite(RS, 1);
	fastFill(color,(p2.x-p1.x+1)*(p2.y-p1.y+1));
	digitalWrite(CS, 1);
}

void fillRoundRect(Point p1, Point p2, RGB color) {
	if (p1.x > p2.x)
		swap(int, p1.x, p2.x);

	if (p1.y > p2.y)
		swap(int, p1.y, p2.y);

	if ((p2.x - p1.x) > 4 && (p2.y - p1.y) > 4) {
		for (int i=0; i<((p2.y - p1.y)/2)+1; i++) {
			switch(i){
			case 0:
				drawHLine(Point(p1.x + 2, p1.y + i), p2.x - p1.x - 4, color);
				drawHLine(Point(p1.x + 2, p2.y - i), p2.x - p1.x - 4, color);
				break;
			case 1:
				drawHLine(Point(p1.x + 1, p1.y + i), p2.x - p1.x - 2, color);
				drawHLine(Point(p1.x + 1, p2.y - i), p2.x - p1.x - 2, color);
				break;
			default:
				drawHLine(Point(p1.x, p1.y + i), p2.x - p1.x, color);
				drawHLine(Point(p1.x, p2.y - i), p2.x - p1.x, color);
			}
		}
	}
}

void drawCircle(Point c, int radius, RGB color) {
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x1 = 0;
	int y1 = radius;

	digitalWrite(CS, 0);
	setArea(Point(c.x, c.y + radius), Point(c.x, c.y + radius));
	sendData(getColor16b(color));
	setArea(Point(c.x, c.y - radius), Point(c.x, c.y - radius));
	sendData(getColor16b(color));
	setArea(Point(c.x + radius, c.y), Point(c.x + radius, c.y));
	sendData(getColor16b(color));
	setArea(Point(c.x - radius, c.y), Point(c.x - radius, c.y));
	sendData(getColor16b(color));

	while(x1 < y1)
	{
		if(f >= 0)
		{
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		x1++;
		ddF_x += 2;
		f += ddF_x;
		setArea(Point(c.x + x1, c.y + y1), Point(c.x + x1, c.y + y1));
		sendData(getColor16b(color));
		setArea(Point(c.x - x1, c.y + y1), Point(c.x - x1, c.y + y1));
		sendData(getColor16b(color));
		setArea(Point(c.x + x1, c.y - y1), Point(c.x + x1, c.y - y1));
		sendData(getColor16b(color));
		setArea(Point(c.x - x1, c.y - y1), Point(c.x - x1, c.y - y1));
		sendData(getColor16b(color));
		setArea(Point(c.x + y1, c.y + x1), Point(c.x + y1, c.y + x1));
		sendData(getColor16b(color));
		setArea(Point(c.x - y1, c.y + x1), Point(c.x - y1, c.y + x1));
		sendData(getColor16b(color));
		setArea(Point(c.x + y1, c.y - x1), Point(c.x + y1, c.y - x1));
		sendData(getColor16b(color));
		setArea(Point(c.x - y1, c.y - x1), Point(c.x - y1, c.y - x1));
		sendData(getColor16b(color));
	}

	digitalWrite(CS, 1);
	resetArea();
}

void fillCircle(Point c, int r, RGB color) {
	int x1, y1;
	for(y1 = -r; y1 <= 0; y1++)
		for(x1 = -r; x1 <= 0; x1++)
			if(x1 * x1 + y1 * y1 <= r * r)
			{
				drawHLine(Point(c.x + x1, c.y + y1), 2*(-x1), color);
				drawHLine(Point(c.x + x1, c.y - y1), 2*(-x1), color);
				break;
			}
}

void clearScreen() {
	digitalWrite(CS, 0);
	resetArea();
	digitalWrite(RS, 1);
	fastFill(COLOR_BLACK, (disp_x_size + 1) * (disp_y_size + 1));
	digitalWrite(CS, 1);
}

void fillScreen(RGB color) {
	digitalWrite(CS, 0);
	resetArea();
	digitalWrite(RS, 1);
	fastFill(color, ((disp_x_size+1)*(disp_y_size+1)));
	digitalWrite(CS, 1);
}

void printChar(uint8_t c, Point p, Font font, RGB color, RGB background) {
	uint8_t i,ch;
	uint16_t j;
	uint16_t temp;

	digitalWrite(CS, 0);

	if (&background != NULL)
	{
		if (orient==PORTRAIT)
		{
			setArea(p, Point(p.x + font.x_size - 1, p.y + font.y_size - 1));
			temp=((c - font.offset) * ((font.x_size / 8) * font.y_size)) + 4;

			for(j = 0; j < ((font.x_size / 8) * font.y_size); j++)
			{
				ch = pgm_read_byte(&font.chars[temp]);
				for(i = 0; i < 8; i++)
				{
					if((ch & (1 << (7 - i))) != 0)
						sendData(getColor16b(color));
					else
						sendData(getColor16b(background));
				}
				temp++;
			}
		}
		else
		{
			temp = ((c - font.offset) * ((font.x_size / 8) * font.y_size)) + 4;

			for(j = 0; j < ((font.x_size / 8) * font.y_size); j += (font.x_size / 8))
			{
				setArea(Point(p.x, p.y + (j / (font.x_size / 8))), Point(p.x + font.x_size - 1, p.y + (j / (font.x_size / 8))));
				for (int zz = (font.x_size / 8) - 1; zz >= 0; zz--)
				{
					ch=pgm_read_byte(&font.chars[temp + zz]);
					for(i = 0; i < 8; i++)
					{
						if((ch & (1 << i)) != 0)
							sendData(getColor16b(color));
						else
							sendData(getColor16b(background));
					}
				}
				temp += (font.x_size / 8);
			}
		}
	}
	else
	{
		temp = ((c - font.offset) * ((font.x_size / 8) * font.y_size)) + 4;
		for(j = 0; j < font.y_size; j++)
		{
			int zz;
			for (zz =0; zz < (font.x_size / 8); zz++)
			{
				ch = pgm_read_byte(&font.chars[temp + zz]);
				for(i = 0; i < 8; i++)
				{
					if((ch & (1 << (7 - i))) != 0)
					{
						setArea(Point(p.x + i + (zz * 8), p.y + j), Point(p.x + i + (zz * 8) + 1, p.y + j + 1));
						sendData(getColor16b(color));
					}
				}
			}
			temp+=(font.x_size / 8);
		}
	}

	digitalWrite(CS, 1);
	resetArea();
}

void rotateChar(uint8_t c, Point p, Font font, int pos, int deg, RGB color, RGB background) {
	uint8_t i,j,ch;
	uint16_t temp;
	int newx, newy;
	double radian;
	radian = deg * 0.0175;

	digitalWrite(CS, 0);

	temp=((c - font.offset) * ((font.x_size / 8) * font.y_size)) + 4;

	for(j = 0; j < font.y_size; j++)
	{
		for (int zz = 0; zz < (font.x_size / 8); zz++)
		{
			ch=pgm_read_byte(&font.chars[temp + zz]);
			for(i = 0; i < 8; i++)
			{
				newx = p.x + (((i + (zz * 8) + (pos * font.x_size)) * cos(radian)) - (j * sin(radian)));
				newy = p.y + ((j * cos(radian)) + ((i + (zz * 8) + (pos * font.x_size)) * sin(radian)));

				setArea(Point(newx, newy), Point(newx+1, newy+1));

				uint8_t ch = (uint8_t) (getColor16b(color) >> 8);
				if((ch & (1 << (7 - i))) != 0)
					sendData(getColor16b(color));
				else if (&background != NULL)
					sendData(getColor16b(background));
			}
		}
		temp += (font.x_size / 8);
	}

	digitalWrite(CS, 1);
	resetArea();
}

void printStr(char * str, Point p, FontType fontType, int deg, RGB color, RGB background) {
	int stl, i;
	stl = strlen(str);
	Font font = getFont(fontType);

	if (orient==PORTRAIT)
	{
		if (p.x == RIGHT)
			p.x = (disp_x_size + 1) - (stl * font.x_size);
		if (p.x == CENTER)
			p.x = ((disp_x_size + 1) - (stl * font.x_size)) / 2;
	}
	else
	{
		if (p.x == RIGHT)
			p.x = (disp_y_size + 1) - (stl * font.x_size);
		if (p.x == CENTER)
			p.x = ((disp_y_size + 1) - (stl * font.x_size)) / 2;
	}

	for (i = 0; i < stl; i++) {
		if (deg == 0)
			printChar(*str++, Point(p.x + (i * font.x_size), p.y), font, color, background);
		else
			rotateChar(*str++, p, font, i, deg, color, background);
	}
}

void printInt(long num, Point p, FontType fontType, int length, char filler, RGB color, RGB background) {
	char buf[25];
	char str[27];
	bool neg = false;
	int i, c=0, f=0;

	if (num == 0)
	{
		if (length != 0)
		{
			for (c=0; c < (length - 1); c++)
				str[c] = filler;
			str[c] = 48;
			str[c + 1] = 0;
		}
		else
		{
			str[0] = 48;
			str[1] = 0;
		}
	}
	else
	{
		if (num < 0)
		{
			neg = true;
			num =- num;
		}
		while (num > 0)
		{
			buf[c]=48+(num % 10);
			c++;
			num=(num - (num % 10))/10;
		}

		buf[c]=0;
		if (neg)
			str[0]=45;
		if (length > (c + neg))
		{
			for (i = 0; i < (length - c - neg); i++)
			{
				str[i + neg] = filler;
				f++;
			}
		}
		for (i = 0; i < c; i++)
			str[i + neg + f] = buf[c - i - 1];
		str[c + neg + f]=0;
	}

	printStr(str, p, fontType, 0, color, background);
}

void printFloat(double num, uint8_t dec, Point p, FontType fontType, char divider, int length, char filler, RGB color, RGB background) {
	char str[27];
	bool neg = false;
	int i;

	if (dec < 1)
		dec = 1;
	else if (dec > 5)
		dec = 5;
	if (num < 0)
		neg = true;
	char* format = "%.2f";
	format[2] = 48 + (dec%10);
	snprintf(str, length, format, num);

	if (divider != '.')
		for (i = 0; i < sizeof(str); i++)
			if (str[i] == '.')
				str[i] = divider;

	if (filler != ' ')
	{
		if (neg)
		{
			str[0] = '-';
			for (i = 1; i < sizeof(str); i++)
				if ((str[i] == ' ') || (str[i] == '-'))
					str[i] = filler;
		}
		else{
			for (i = 0; i < sizeof(str); i++)
				if (str[i] == ' ')
					str[i] = filler;
		}
	}

	printStr(str, p, fontType, 0, color, background);
}

void drawBitmap(Point p, int width, int height, unsigned int* data, int scale) {
	unsigned int col;
	int tx, ty, tc, tsx, tsy;

	if (scale == 1)
	{
		if (orient==PORTRAIT)
		{
			digitalWrite(CS, 0);
			setArea(p, Point(p.x + width - 1, p.y + height - 1));

			for (tc = 0; tc < (width * height); tc++)
			{
				col=pgm_read_word(&data[tc]);
				sendData(col);
			}
			digitalWrite(CS, 1);
		}
		else
		{
			digitalWrite(CS, 0);
			for (ty = 0; ty < height; ty++)
			{
				setArea(Point(p.x, p.y + ty), Point(p.x + width - 1, p.y + ty));
				for (tx = width - 1; tx >= 0; tx--)
				{
					col=pgm_read_word(&data[(ty*width)+tx]);
					sendData(col);
				}
			}
			digitalWrite(CS, 1);
		}
	}
	else
	{
		if (orient==PORTRAIT)
		{
			digitalWrite(CS, 0);
			for (ty = 0; ty < height; ty++)
			{
				setArea(Point(p.x, p.y + (ty * scale)), Point(p.x + ((width * scale) - 1), p.y + (ty * scale) + scale));
				for (tsy = 0; tsy < scale; tsy++)
					for (tx = 0; tx < width; tx++)
					{
						col=pgm_read_word(&data[(ty*width)+tx]);
						for (tsx = 0; tsx < scale; tsx++)
							sendData(col);
					}
			}
			digitalWrite(CS, 1);
		}
		else
		{
			digitalWrite(CS, 0);
			for (ty = 0; ty < height; ty++)
			{
				for (tsy = 0; tsy < scale; tsy++)
				{
					setArea(Point(p.x, p.y + (ty*scale) + tsy), Point(p.x + ((width * scale) - 1), p.y + (ty * scale) + tsy));
					for (tx = width - 1; tx >= 0; tx--)
					{
						col=pgm_read_word(&data[(ty*width)+tx]);
						for (tsx=0; tsx<scale; tsx++)
							sendData(col);
					}
				}
			}
			digitalWrite(CS, 1);
		}
	}
	resetArea();
}
