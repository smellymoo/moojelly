#pragma once

#include <cstdint>  // uint16_t
#include <vector>	// sprites

#define BANK_SIZE       0x4000
#define ROM_SIZE        512*1024

#define LEVEL_WIDTH     256
#define LEVEL_HEIGHT    48
#define LEVEL_SIZE      (LEVEL_WIDTH * LEVEL_HEIGHT)

#define CALLV(FUNC) if (game == 2) SML2::FUNC; else SML3::FUNC
#define LEVELS	    (ROM::game == 2 ? 32 : 43)

typedef unsigned char byte;

struct point16 { uint16_t y, x; };

struct RGB_DATA { byte r, g, b; };

struct Bitmap {
    RGB_DATA * RGB;
    int width, height;
    bool auto_delete = true;

    Bitmap() {};
    Bitmap(int pWidth, int pHeight, bool stack) : width(pWidth), height(pHeight), auto_delete(stack) { RGB = new RGB_DATA[width * height]{}; }
    ~Bitmap() { if (auto_delete) delete[] RGB; }
};

struct sprite {
   uint16_t x, y;
   byte type;

   sprite(uint16_t pX, uint16_t pY, byte t) : x(pX), y(pY), type(t) {} //SML2
   sprite(unsigned int p, byte t) : type(t) { x = (p & 0xFF) * 2 + 1; y = ((p >> 8) + 17) * 2; } //SML3
};
