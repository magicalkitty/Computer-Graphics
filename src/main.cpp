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
#include "Mirror.h"
#include <memory>
#include <random>

float randomOffset() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

int main() {
    int nx = 900;
    int ny = 900;
    Framebuffer fb(nx, ny);
    
    PerspectiveCamera cam(nx, ny, 1.5f);
    
    Scene world;
    
    Light light(vec3(20.0, 10.0, 5.0), vec3(1.0, 1.0, 1.0));
    int rpp_NSquare = 4;

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
    fb.exportToPNG("perspective_camera_output");

    fb.clearToColor(color(0.0, 0.0, 0.0));

    world.add(std::make_shared<Sphere>(point3(0, -1.25, -7), 0.3));
    world.add(std::make_shared<Sphere>(point3(0, 0, -10), 3));
    world.add(std::make_shared<Sphere>(point3(0, 0, -15), 5));

     for (int x=0; x<fb.get_width(); ++x) {
        for (int y=0; y<fb.get_height(); ++y) {
            ray r;
            cam.generateRay(x, y, r);

            color c = world.computeRayColor(r, 1.0, INFINITY, light, 10);

            fb.setPixelColor(x, y, c);
        }
    }

    fb.exportToPNG("multiple_spheres_output");
    
    
    fb.clearToColor(color(0.0, 0.0, 0.0));

    world.clear();

    shared_ptr<Shader> lambertShaderTri = make_shared<Lambertian>(color(230.0/255.0, 126.0/255.0, 219.0/255.0));
    shared_ptr<Shader> BlinnPhongShaderTri = make_shared<BlinnPhong>(color(230.0/255.0, 126.0/255.0, 219.0/255.0), 64.0f);

    world.add(std::make_shared<Triangle>(point3(-1.2, -0.2, -7), point3(0.8, -0.5, -5), point3(0.9, 0.0, -5), lambertShaderTri));
    world.add(std::make_shared<Triangle>(point3(0.773205, -0.93923, -7), point3(0.0330127, 0.94282, -5), point3(-0.45, 0.779423, -5), BlinnPhongShaderTri));
    world.add(std::make_shared<Triangle>(point3(0.426795, 1.13923, -7), point3(-0.833013, -0.44282, -5), point3(-0.45, -0.779423, -5)));
    
    for (int x=0; x<fb.get_width(); ++x) {
        for (int y=0; y<fb.get_height(); ++y) {
            ray r;
            cam.generateRay(x, y, r);

            color c = world.computeRayColor(r, 1.0, INFINITY, light, 10);

            fb.setPixelColor(x, y, c);
        }
    
    }
    fb.exportToPNG("triangle_intersection_output");

    world.clear();

    shared_ptr<Shader> lambertShader = make_shared<Lambertian>(color(115.0/255.0, 192.0/255.0, 209.0/255.0));
    shared_ptr<Shader> BlinnPhongShader = make_shared<BlinnPhong>(color(115.0/255.0, 192.0/255.0, 209.0/255.0));
    shared_ptr<Shader> mirrorShader = make_shared<Mirror>();

    shared_ptr<Sphere> sphere = make_shared<Sphere>(point3(2.5, 0.0, -40.0), 2.0, BlinnPhongShader);
    shared_ptr<Sphere> sphere2 = make_shared<Sphere>(point3(12.5, 2.0, -50.0), 4.0, lambertShader);
    shared_ptr<Sphere> sphere3 = make_shared<Sphere>(point3(-2.5, -1.0, -35.0), 1.0);
    shared_ptr<Sphere> sphere4 = make_shared<Sphere>(point3(0.0, -600.0, -700.0), 650.0, lambertShader);

    shared_ptr<Triangle> triangle1 = make_shared<Triangle>(point3(0.0, -7.5, 30.0), point3(500.0, -7.5, -1000.0), point3(-500.0, -7.5, -1000.0), lambertShaderTri);

    world.add(sphere);
    world.add(sphere2);
    world.add(sphere3);
    //world.add(sphere4);
    world.add(triangle1);

    for (int x=0; x<fb.get_width(); ++x) {
        for (int y=0; y<fb.get_height(); ++y) {
            color c(0.0, 0.0, 0.0);

            for (int p=0; p<rpp_NSquare; ++p) {
                for (int q=0; q<rpp_NSquare; ++q) {
                    float tmin = 1.0;
                    float tmax = std::numeric_limits<float>::infinity();
                    
                    float pOffset = (p + randomOffset())/ rpp_NSquare;
                    float qOffset = (q + randomOffset())/ rpp_NSquare;

                    ray r;
                    cam.generateRay(x + pOffset, y + qOffset, r);
                    c += world.computeRayColor(r, tmin, tmax, light, 10);

                }
            }

            c /= (rpp_NSquare * rpp_NSquare);
            fb.setPixelColor(x, y, c);
        }
    }

    fb.exportToPNG("Testing");

    world.clear();

    shared_ptr<Sphere> sphere6 = make_shared<Sphere>(point3(2.5, 0.0, -40.0), 2.0, mirrorShader);
    shared_ptr<Sphere> sphere7 = make_shared<Sphere>(point3(12.5, 2.0, -50.0), 4.0, BlinnPhongShader);
    shared_ptr<Sphere> sphere8 = make_shared<Sphere>(point3(-2.5, -1.0, -35.0), 1.0);
    world.add(sphere6);
    world.add(sphere7);
    world.add(sphere8);
    shared_ptr<Triangle> triangle5 = make_shared<Triangle>(point3(0.0, -7.5, 30.0), point3(500.0, -7.5, -1000.0), point3(-500.0, -7.5, -1000.0), lambertShaderTri);
    world.add(triangle5);
    
    for (int x=0; x<fb.get_width(); ++x) {
        for (int y=0; y<fb.get_height(); ++y) {
            ray r;
            cam.generateRay(x, y, r);

            color c = world.computeRayColor(r, 1.0, INFINITY, light, 8);

            fb.setPixelColor(x, y, c);
        }
    }

    fb.exportToPNG("Testing_mirror");

    world.clear();

    shared_ptr<Sphere> sphere9 = make_shared<Sphere>(point3(0.0, 0.0, -10.0), 1.0, BlinnPhongShader);
    world.add(sphere9);

    for (int x=0; x<fb.get_width(); ++x) {
        for (int y=0; y<fb.get_height(); ++y) {
            color c(0.0, 0.0, 0.0);

            for (int p=0; p<rpp_NSquare; ++p) {
                for (int q=0; q<rpp_NSquare; ++q) {
                    float tmin = 1.0;
                    float tmax = std::numeric_limits<float>::infinity();
                    
                    float pOffset = (p + randomOffset())/ rpp_NSquare;
                    float qOffset = (q + randomOffset())/ rpp_NSquare;

                    ray r;
                    cam.generateRay(x + pOffset, y + qOffset, r);
                    c += world.computeRayColor(r, tmin, tmax, light, 10);

                }
            }

            c /= (rpp_NSquare * rpp_NSquare);
            fb.setPixelColor(x, y, c);
        }
    }

    fb.exportToPNG("Testing_antialiasing");

    
    return 0;
}
