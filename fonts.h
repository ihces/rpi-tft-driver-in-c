#ifndef FONTS_h
#define FONTS_h

#include <stdint.h>

typedef struct {
        uint8_t* chars;
        uint8_t x_size;
        uint8_t y_size;
        uint8_t offset;
        uint8_t numchars;
}Font;

typedef enum {
        SMALL,
        SEVEN_SEGMENT
}FontType;

Font getFont(FontType fontType);

#endif
