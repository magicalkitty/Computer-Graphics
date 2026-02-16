#include "Framebuffer.h"
#include "../renderlib/vec3.h"
#include "PerspectiveCamera.h"
#include "Sphere.h"
#include "Ray.h"
#include "triangle.h"
#include "Scene.h"
#include "Light.h"
#include "Lambertian.h"
#include "BlinnPhong.h"
#include <memory>

color ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if (world.intersect(r, 0, INFINITY, rec)) {
        return 0.5 * (rec.normal + color(1,1,1));
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

int main() {
    int nx = 900;
    int ny = 900;
    Framebuffer fb(nx, ny);
        
    PerspectiveCamera cam(nx, ny, 0.6f);

    Scene world;


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

    // PerspectiveCamera cam(nx, ny, 1.0f);

    fb.clearToColor(color(0.0, 0.0, 0.0));

    point3 center(0.0, 0.0, -10.0);
    double radius = 1.0;
    // Sphere s(center, radius);

    // for (int x=0; x<fb.get_width(); ++x) {
    //     for (int y=0; y<fb.get_height(); ++y) {
    //         ray r;
    //         cam.generateRay(x, y, r);
    //         hit_record rec;
    //         if (s.intersect(r, 1, INFINITY, rec)) {
    //             fb.setPixelColor(x, y, color(1.0, 0.0, 0.0));
    //         }
    //         else {
    //             fb.setPixelColor(x, y, color(1.0, 1.0, 1.0));
    //         }
    //     }
    // }

    
    // fb.exportToPNG("sphere_intersection_output");

    fb.clearToColor(color(0.0, 0.0, 0.0));
    
    // Sphere s_1(point3(0, -1.25, -7), 0.3);
    // Sphere s_2(point3(0, 0, -10), 3);
    // Sphere s_3(point3(0, 0, -15), 5);

    // world.add(std::make_shared<Sphere>(point3(0, -1.25, -7), 0.3));
    // world.add(std::make_shared<Sphere>(point3(0, 0, -10), 3));
    // world.add(std::make_shared<Sphere>(point3(0, 0, -15), 5));

     for (int x=0; x<fb.get_width(); ++x) {
        for (int y=0; y<fb.get_height(); ++y) {
            ray r;
            cam.generateRay(x, y, r);
            hit_record rec;
            fb.setPixelColor(x, y, ray_color(r, world)); // default to white
            // if (s_1.intersect(r, 1, INFINITY, rec)) {
            //     fb.setPixelColor(x, y, color(0.992, 0.863, 0.239));
            // }
            // else if (s_2.intersect(r, 1, INFINITY, rec)) {
            //     fb.setPixelColor(x, y, color(0.149, 0.451, 0.698));
            // }
            // else if (s_3.intersect(r, 1, INFINITY, rec)) {
            //     fb.setPixelColor(x, y, color(0.992, 0.863, 0.239));
            // }
            // else {
            //     fb.setPixelColor(x, y, color(1.0, 1.0, 1.0));
            // }
        }
    }

    fb.exportToPNG("multiple_spheres_output");
    
    
    fb.clearToColor(color(0.0, 0.0, 0.0));

    // point3 a(-1.2, -0.2, -17.0);
    // point3 b(0.8, -0.5, -15.0);
    // point3 c(0.9, 0.0, -15.0);
    
    // Triangle tri1(a, b, c);

    // Triangle tri1(point3(-1.2, -0.2, -7), point3(0.8, -0.5, -5), point3(0.9, 0.0, -5));

    // Triangle tri2(point3(0.773205, -0.93923, -7), point3(0.0330127, 0.94282, -5), point3(-0.45, 0.779423, -5));

    // Triangle tri3(point3(0.426795, 1.13923, -7), point3(-0.833013, -0.44282, -5), point3(-0.45, -0.779423, -5));

    world.add(std::make_shared<Triangle>(point3(-1.2, -0.2, -7), point3(0.8, -0.5, -5), point3(0.9, 0.0, -5)));
    world.add(std::make_shared<Triangle>(point3(0.773205, -0.93923, -7), point3(0.0330127, 0.94282, -5), point3(-0.45, 0.779423, -5)));
    world.add(std::make_shared<Triangle>(point3(0.426795, 1.13923, -7), point3(-0.833013, -0.44282, -5), point3(-0.45, -0.779423, -5)));
    
    for (int x=0; x<fb.get_width(); ++x) {
        for (int y=0; y<fb.get_height(); ++y) {
            ray r;
            cam.generateRay(x, y, r);
            hit_record rec;

            fb.setPixelColor(x, y, ray_color(r, world)); // default to white
            // if (tri1.intersect(r, 0.001, INFINITY, rec)) {
            //     fb.setPixelColor(x, y, color(1.0, 0.0, 0.0));
            // }
            // else if (tri2.intersect(r, 0.001, INFINITY, rec)) {
            //     fb.setPixelColor(x, y, color(0.0, 1.0, 0.0));
            // }
            // else if (tri3.intersect(r, 0.001, INFINITY, rec)) {
            //     fb.setPixelColor(x, y, color(0.0, 0.0, 1.0));
            // }
            // else {
            //     fb.setPixelColor(x, y, color(1.0, 1.0, 1.0));
            // }
        }
    
    }
    fb.exportToPNG("triangle_intersection_output");

    world.clear();
    Light light(vec3(30.0, 10.0, 5.0), vec3(1.0, 1.0, 1.0));

    shared_ptr<Shader> lambertShader = make_shared<Lambertian>(color(115.0/255.0, 192.0/255.0, 209.0/255.0));
    shared_ptr<Shader> BlinnPhongShader = make_shared<BlinnPhong>(color(115.0/255.0, 192.0/255.0, 209.0/255.0));

    shared_ptr<Sphere> sphere = make_shared<Sphere>(point3(0.0, 0.0, -15.0), 2.0, BlinnPhongShader);
    sphere->setShader(BlinnPhongShader);

    world.add(sphere);

    for (int x=0; x<fb.get_width(); ++x) {
        for (int y=0; y<fb.get_height(); ++y) {
            ray r;
            cam.generateRay(x, y, r);

            color c = world.computeRayColor(r, 1.0, INFINITY, light);

            fb.setPixelColor(x, y, c);
        }
    }

    fb.exportToPNG("Testing");

    // world.add(std::make_shared<Sphere>(point3(0.0, 0.0, -15.0), 1.0));
    // fb.clearToColor(vec3(1.0, 1.0, 1.0));
    // Sphere s(point3(0.0, 0.0, -15.0), 1.0, vec3(0.0, 0.0, 1.0));
    // for (int x=0; x<fb.get_width(); ++x) {
    //     for (int y=0; y<fb.get_height(); ++y) {
    //         ray r;
    //         cam.generateRay(x, y, r);
    //         hit_record rec;

    //         if (s.intersect(r, 1, INFINITY, rec)) {
    //             fb.setPixelColor(x, y, s.get_color());
    //         }
    //         else {
    //             fb.setPixelColor(x, y, color(1.0, 1.0, 1.0));
    //         }

    //     }
    // }
    // fb.exportToPNG("Testing");

    // world.clear();

    // Light light(vec3(0, 5, 10), vec3(1, 1, 1));

    // auto redShader = std::make_shared<Lambertian>(vec3(1,0,0));
    // auto sphere1 = std::make_shared<Sphere>(center, radius, redShader);
    // world.add(sphere1);


    // for (int x=0; x<fb.get_width(); ++x) {
    //     for (int y=0; y<fb.get_height(); ++y) {
    //         ray r;
    //         cam.generateRay(x, y, r);
    //         color c = color(1.0, 0.0, 0.0);
    //         // color c = world.computeRayColor(r, 1, INFINITY, light);

    //         fb.setPixelColor(x, y, c);
    //     }
    // }

    // fb.exportToPNG("Lambertian");


    
    return 0;
}
