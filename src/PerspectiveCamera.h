#pragma once

#include "Camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class PerspectiveCamera : public Camera{
    public:

    PerspectiveCamera();

    PerspectiveCamera(int nx, int ny, float fov);

    PerspectiveCamera(vec3 position, int nx, int ny, float fov);

    PerspectiveCamera(vec3 position, vec3 viewDir, int nx, int ny, float fov);

    ray generateRay(float i, float j, ray &r0) override;

    PerspectiveCamera(glm::vec3 position, glm::vec3 viewDir, int nx, int ny, float fov);

    glm::mat4 getProjectionMatrix() const;

    private:
        float l, r, b, t; //image plane coords
};