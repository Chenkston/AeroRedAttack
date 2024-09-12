#ifndef REDCOUNTER_H
#define REDCOUNTER_H

#include <iostream>
#include <png.h>
#include "redCounter.cpp"

int readPNGforRed(const char* filename);
int processPNG(png_structp png, png_infop info);
bool countRedPixels(png_bytep pixel);

#endif // REDCOUNTER_H