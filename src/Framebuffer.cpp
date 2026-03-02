#include "Framebuffer.h"
#include "png++/png.hpp"
#include <algorithm>

Framebuffer::Framebuffer()
    : width(100), height(100), fbStorage(width * height) {}

Framebuffer::Framebuffer(int w, int h)
    : width(w), height(h), fbStorage(width * height) {}

void Framebuffer::clearToColor(const color c)
{
    for (auto idx=0u; idx<fbStorage.size(); ++idx) {
        fbStorage[idx] = c;
    }
}

void Framebuffer::clearToGradient(const color topColor, const color bottomColor)
{
    for (int x=0; x<width; ++x) {
        for (int y=0; y<height; ++y) {
            auto idx = y * width + x;
            auto t = double(y) / height;

            color c = (1-t) * topColor + t * bottomColor;
            fbStorage[idx] = c;
        }
        
    }
}

void Framebuffer::setPixelColor(int i, int j, const color c)
{
    // Input Validator
    fbStorage[j * width + i] = c;
}

void Framebuffer::setPixelColor(int index, const color c)
{
    // Input Validator
    fbStorage[index] = c;
}

void Framebuffer::exportToPNG(std::string filename)
{
    // Create an image of size (width x height)
    png::image<png::rgb_pixel> imData(width, height);

    // Fill in the image data
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            color c = fbStorage[y*width+x];
            png::byte r = static_cast<png::byte>(std::clamp(c.x() * 255.0, 0.0, 255.0));
            png::byte g = static_cast<png::byte>(std::clamp(c.y() * 255.0, 0.0, 255.0));
            png::byte b = static_cast<png::byte>(std::clamp(c.z() * 255.0, 0.0, 255.0));
            // The origin for indexing the height is in lower left...
            imData[height - 1 - y][x] = png::rgb_pixel( r, g, b );
        }
    }
    imData.write(filename + ".png");
}
