#include "Framebuffer.h"
#include "../renderlib/vec3.h"
#include "PerspectiveCamera.h"
#include "Sphere.h"
#include "Ray.h"

int main() {
    int nx = 900;
    int ny = 600;
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

    fb.clearToColor(color(0.0, 0.0, 0.0));

    point3 center(0.0, 0.0, -10.0);
    double radius = 1.0;
    Sphere s(center, radius);

    for (int x=0; x<fb.get_width(); ++x) {
        for (int y=0; y<fb.get_height(); ++y) {
            ray r;
            cam.generateRay(x, y, r);
            hit_record rec;
            if (s.hit(r, 0.001, INFINITY, rec)) {
                fb.setPixelColor(x, y, color(1.0, 0.0, 0.0));
            }
            else {
                fb.setPixelColor(x, y, color(1.0, 1.0, 1.0));
            }
        }
    }

    fb.exportToPNG("sphere_intersection_output");
    
    return 0;
}
