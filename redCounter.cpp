#include <iostream>
#include <png.h>
#include <algorithm>

/**
 * isRedPixel takes in a pixel and checks if it counts as a red one.
 */
bool isRedPixel(png_bytep pixel) {
    // Extract RGB values from the pixel
    int red = pixel[0];
    int green = pixel[1];
    int blue = pixel[2];

    // Convert RGB to HSV
    double r = red / 255.0;
    double g = green / 255.0;
    double b = blue / 255.0;

    double maxVal = std::max(r, g);
    maxVal = std::max(maxVal, b);
    double minVal = std::min(r, g);
    minVal = std::min(minVal, b);

    double delta = maxVal - minVal;

    double hue = 0.0;
    if (delta == 0) {
        hue = 0.0;
    } else if (maxVal == r) {
        hue = 60.0 * (((g - b) / (delta)));
    } else if (maxVal == g) {
        hue = 60.0 * (2.0 + (b - r) / (delta));
    } else if (maxVal == b) {
        hue = 60.0 * (4.0 + (r - g) / (delta));
    }

    if (hue < 0.0) {   
        hue += 360.0;
    }

    double saturation = 0.0;
    if (maxVal != 0.0) {
        saturation = (maxVal - minVal) / maxVal;
    }

    double value = maxVal;

    // Check if the HSV pixels match the criteria for a red pixel.
    if(saturation*255 >= 100 && saturation*255 <= 255) {
        if(value*255 >= 100 && value*255 <= 255) {
            if(hue >= 0 && hue <= 20) {
                return true;
            } else if (hue >= 320 && hue <= 360) {  //I've doubled the values to fit the whole 360 hue range
                return true;
            }
        }
    }

    return false;
}

/**
 * processPNG reads the given png, and counts the number of red pixels.
 */
int processPNG(png_structp png, png_infop info) {
    int width = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);

    png_bytep* row_pointers = new png_bytep[height];
    for (int y = 0; y < height; y++) {
        row_pointers[y] = new png_byte[png_get_rowbytes(png, info)];
    }

    png_read_image(png, row_pointers);

    int redCount = 0;
    // Process each pixel
    for (int y = 0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width; x++) {
            png_bytep pixel = &(row[x * 4]); // Assuming RGBA format
            
            if(isRedPixel(pixel)) {
                redCount++;
            }
        }
    }

    return redCount;
}

/**
 * readPNGforRed takes in a PNG file and returns the number of red pixels.
 */
int readPNGforRed(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 0;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        std::cerr << "Error creating PNG read struct" << std::endl;
        fclose(file);
        return 0;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        std::cerr << "Error creating PNG info struct" << std::endl;
        png_destroy_read_struct(&png, nullptr, nullptr);
        fclose(file);
        return 0;
    }

    if (setjmp(png_jmpbuf(png))) {
        std::cerr << "Error during PNG read" << std::endl;
        png_destroy_read_struct(&png, &info, nullptr);
        fclose(file);
        return 0;
    }

    png_init_io(png, file);
    png_read_info(png, info);

    //get the number of red Pixels
    int redCount = processPNG(png, info);

    png_destroy_read_struct(&png, &info, nullptr);
    fclose(file);

    return redCount;
}

