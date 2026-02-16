#pragma once

#include "../renderlib/vec3.h"

class Light {
    public:
        Light(const vec3 position, const vec3 color) : position(position), color(color) {}

        const vec3& getPosition() const { return position; }
        const vec3& getColor() const { return color; }

    private:
        vec3 position;
        vec3 color;
};