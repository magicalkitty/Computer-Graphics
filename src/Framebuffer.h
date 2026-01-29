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

    void clearToColor(color color);

    void setPixelColor(int i, int j, color c);
    void setPixelColor(int index, color c);

    // my fb is vec3... how do i convert from [0,1] floats to 255
    void exportToPNG(std::string filename);

    private:
    int width, height;
    std::vector<color> fbStorage;
};