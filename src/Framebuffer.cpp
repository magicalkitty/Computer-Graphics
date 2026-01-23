#include "Framebuffer.h"

Framebuffer::Framebuffer()
    : width(100), height(100), fbStorage(width * height)
{
}

Framebuffer::Framebuffer(int w, int h)
    : width(w), height(h), fbStorage(width * height)
{
}

void Framebuffer::clearToColor(color c)
{
    for (auto idx=0u; idx<fbStorage.size(); ++idx) {
        fbStorage[idx] = c;
    }
}