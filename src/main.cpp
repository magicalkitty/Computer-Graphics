#include "Framebuffer.h"
#include "../renderlib/vec3.h"
#include "PerspectiveCamera.h"

int main() {
    int nx = 200;
    int ny = 200;
    Framebuffer fb(nx, ny);
        
    PerspectiveCamera cam;

    fb.clearToColor(color(0.0, 0.0, 0.0));
    for (int j = 0; j < ny; ++j) {
        for (int i = 0; i < nx; ++i) {
            ray r;
            cam.generateRay(i, j, r);

            vec3 raydir = unit_vector(r.direction());

            color pixelColor(0.5*(raydir.x()+1.0f),
                            0.5*(raydir.y()+1.0f),
                            0.5*(raydir.z()+1.0f));

            fb.setPixelColor(i, j, pixelColor);
        }
    }

        // fb.exportToPNG("perspective_camera_output");
        // return 0;
    fb.exportToPNG("perspective_camera_output");
    return 0;
}

    //     vec3 position(0, 0, -1);
    //     vec3 U(1, 0, 0);
    //     vec3 V(0, 1, 0);
    //     vec3 W(0, 0, 1);
    //     float focal = 0.0325f;
    //     float planeWidth = 0.5f;
    //     float planeHeight = 0.5f;

    //     float l = -planeWidth / 2.0f;
    //     float r =  planeWidth / 2.0f;
    //     float b = -planeHeight / 2.0f;
    //     float t =  planeHeight / 2.0f;

    //     for (int j = 0; j < ny; ++j) {
    //         for (int i = 0; i < nx; ++i) {
    //             float u = l + (r - l) * (i + 0.5f) / float(nx);
    //             float v = b + (t - b) * (j + 0.5f) / float(ny);

    //             vec3 dir = -focal * W + u * U + v * V;
    //             dir = unit_vector(dir);

    //             color pixelColor(0.5 + 0.5*dir.x(),
    //                             0.5 + 0.5*dir.y(),
    //                             0.5 + 0.5*dir.z());

    //             fb.setPixelColor(i, j, pixelColor);
    //         }
    //     }

    //     fb.exportToPNG("perspective_camera_output");
    //     return 0;
    // }
