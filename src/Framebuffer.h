#pragma once

#include <vector>
#include <string>
#include "../renderlib/vec3.h"

using color = vec3;

class Framebuffer {
    public:
    // Default constructor -- want a simple width and height
    Framebuffer();
    Framebuffer(int w, int h);

    void clearToColor(const color color);
    void clearToGradient(const color topColor, const color bottomColor);

    void setPixelColor(int i, int j, const color c);
    void setPixelColor(int index, const color c);

    // my fb is vec3... how do i convert from [0,1] floats to 255
    void exportToPNG(const std::string filename);

    int get_width() const { return width; }
    int get_height() const { return height; }

    private:
    int width, height;
    std::vector<color> fbStorage;
};