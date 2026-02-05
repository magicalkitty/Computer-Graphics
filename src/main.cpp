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
