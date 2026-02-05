#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/Framebuffer.h"

TEST_CASE( "Framebuffer setPixelColor and clearToColor test" )
{      
    Framebuffer fb(10, 10);

    fb.clearToColor(color(0.0, 0.0, 1.0));

    fb.setPixelColor(5, 5, color(1.0, 1.0, 1.0));

    fb.exportToPNG("framebuffer_utest_output");
}


TEST_CASE( "Framebuffer - Icelandic Flag" )
{
    Framebuffer fb(300, 200);

    // Base Blue backgroun
    fb.clearToColor(color((2.0/255.0), 82.0/255.0, 156/255.0));

    for (int y = 80; y < 120; ++y) {
        for (int x = 0; x < 300; ++x) {
            fb.setPixelColor(x, y, color(1.0, 1.0, 1.0));
        }
    }

    for (int y = 0; y < 200; ++y) {
        for (int x = 80; x < 120; ++x) {
            fb.setPixelColor(x, y, color(1.0, 1.0, 1.0));
        }
    }

    for (int y = 90; y < 110; ++y) {
        for (int x = 0; x < 300; ++x) {
            fb.setPixelColor(x, y, color(220.0/250.0, 30.0/255.0, 53.0/255.0));
        }
    } 
    for (int y = 0; y < 200; ++y) {
        for (int x = 90; x < 110; ++x) {
            fb.setPixelColor(x, y, color(220.0/250.0, 30.0/255.0, 53.0/255.0));
        }
    } 
    fb.exportToPNG("iceland_flag_output");
}