#ifndef ILI9486_h
#define ILI9486_h

#include <stdint.h>
#include "fonts.h"

#define COLOR_RED (RGB) {255, 0, 0}
#define COLOR_GREEN (RGB) {0, 255, 0}
#define COLOR_BLUE (RGB) {0, 0, 255}
#define COLOR_YELLOW (RGB) {255, 255, 0}
#define COLOR_CYAN (RGB) {0, 255, 255}
#define COLOR_MAGENTA (RGB) {255, 0, 255}
#define COLOR_WHITE (RGB) {255, 255, 255}
#define COLOR_BLACK (RGB) {0, 0, 0}

#define Point(x, y) (Point) {x, y}
#define Color(R, G, B) (RGB) {R, G, B}

typedef struct{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} RGB;

typedef struct{
    uint16_t x;
    uint16_t y;
}Point;

typedef enum{
    LANDSCAPE,
    PORTRAIT
} Orientation;

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

void initScreen (Orientation orientation);

void drawPixel(Point point, RGB color);

void drawLine(Point p1, Point p2, RGB color);

void drawRect (Point p1, Point p2, RGB color);

void drawRoundRect(Point p1, Point p2, RGB color);

void fillRect(Point p1, Point p2, RGB color);

void fillRoundRect(Point p1, Point p2, RGB color);

void drawCircle(Point c, int radius, RGB color);

void fillCircle(Point center, int radius, RGB color);

void clearScreen();

void fillScreen(RGB color);

void printChar(uint8_t c, Point p, Font font, RGB color, RGB background);

void rotateChar(uint8_t c, Point p, Font font, int pos, int deg, RGB color, RGB background);

void printStr(char * str, Point p, FontType fontType, int deg, RGB color, RGB background);

void printInt(long num, Point p, FontType fontType, int length, char filler, RGB color, RGB background);

void printFloat(double num, uint8_t deg, Point p, FontType fontType, char divider, int length, char filler, RGB color, RGB background);

void drawBitmap(Point p, int width, int height, unsigned int* data, int scale);

#endif
