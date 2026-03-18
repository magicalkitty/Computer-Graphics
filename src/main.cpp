#include "Framebuffer.h"
#include "../renderlib/vec3.h"
#include "PerspectiveCamera.h"
#include "Sphere.h"
#include "Ray.h"
#include "Triangle.h"
#include "Scene.h"
#include "Light.h"
#include "Lambertian.h"
#include "BlinnPhong.h"
#include "MirrorShader.h"
#include "DiffuseShader.h"
#include <memory>
#include <random>
#include "handleGraphicsArgs.h"


float randomOffset() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

int main(int argc, char* argv[]) {
    sivelab::GraphicsArgs args;
    args.process(argc, argv);
    int nx = (args.width > 0) ? args.width : 100;
    int ny = (args.height > 0) ? args.height : 100;
    Framebuffer fb(nx, ny);
    
    float focalLength = args.useDepthOfField ? args.depthOfFieldDistance : 1.0f;
    PerspectiveCamera cam(vec3(0, .5, 0), vec3(0, 0, -1), nx, ny, focalLength);
    
    Scene world;

    Light light(vec3(20.0, 10.0, 5.0), vec3(1.0, 1.0, 1.0));

    world.addLight(std::make_shared<Light>(light));

    int rpp_NSquare = (args.rpp > 0) ? args.rpp : 4;

    int recursionDepth = (args.recursionDepth > 0) ? args.recursionDepth : 10;

    fb.clearToGradient(color(204.0/255.0, 126.0/255.0, 88.0/255.0), color(105.0/255.0, 88.0/255.0, 204.0/255.0));
    fb.exportToPNG("gradient_output");

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

    for (int y=0; y<fb.get_height(); ++y) {
        for (int x=0; x<fb.get_width(); ++x) {
            color c(0.0, 0.0, 0.0);

            for (int p=0; p<rpp_NSquare; ++p) {
                for (int q=0; q<rpp_NSquare; ++q) {
                    float tmin = 1.0;
                    float tmax = std::numeric_limits<float>::infinity();
                    
                    float pOffset = (p + randomOffset())/ rpp_NSquare;
                    float qOffset = (q + randomOffset())/ rpp_NSquare;

                    ray r;
                    cam.generateRay(x + pOffset, y + qOffset, r);
                    c += world.computeRayColor(r, tmin, tmax, recursionDepth);

                }
            }

            c /= (rpp_NSquare * rpp_NSquare);
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
    
    for (int y=0; y<fb.get_height(); ++y) {
        for (int x=0; x<fb.get_width(); ++x) {

            ray r;
            cam.generateRay(x, y, r);
            color c = world.computeRayColor(r, 1.0, INFINITY, recursionDepth);

            fb.setPixelColor(x, y, c);
        }
    
    }
    fb.exportToPNG("triangle_intersection_output");

    world.clear();

    shared_ptr<Shader> lambertShader = make_shared<Lambertian>(color(115.0/255.0, 192.0/255.0, 209.0/255.0));
    shared_ptr<Shader> BlinnPhongShader = make_shared<BlinnPhong>(color(115.0/255.0, 192.0/255.0, 209.0/255.0));
    shared_ptr<Shader> mirrorShader = make_shared<MirrorShader>();

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

    for (int y=0; y<fb.get_height(); ++y) {
        for (int x=0; x<fb.get_width(); ++x) {
            color c(0.0, 0.0, 0.0);

            for (int p=0; p<rpp_NSquare; ++p) {
                for (int q=0; q<rpp_NSquare; ++q) {
                    float tmin = 1.0;
                    float tmax = std::numeric_limits<float>::infinity();
                    
                    float pOffset = (p + randomOffset())/ rpp_NSquare;
                    float qOffset = (q + randomOffset())/ rpp_NSquare;

                    ray r;
                    cam.generateRay(x + pOffset, y + qOffset, r);
                    c += world.computeRayColor(r, tmin, tmax, recursionDepth);

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
    
    for (int y=0; y<fb.get_height(); ++y) {
        for (int x=0; x<fb.get_width(); ++x) {
            color c(0.0, 0.0, 0.0);

            for (int p=0; p<rpp_NSquare; ++p) {
                for (int q=0; q<rpp_NSquare; ++q) {
                    float tmin = 1.0;
                    float tmax = std::numeric_limits<float>::infinity();
                    
                    float pOffset = (p + randomOffset())/ rpp_NSquare;
                    float qOffset = (q + randomOffset())/ rpp_NSquare;

                    ray r;
                    cam.generateRay(x + pOffset, y + qOffset, r);
                    c += world.computeRayColor(r, tmin, tmax, recursionDepth);

                }
            }

            c /= (rpp_NSquare * rpp_NSquare);
            fb.setPixelColor(x, y, c);
        }
    }

    fb.exportToPNG("Testing_mirror");

    world.clear();

    shared_ptr<Sphere> sphere9 = make_shared<Sphere>(point3(0.0, 0.0, -10.0), 1.0, lambertShader);
    world.add(sphere9);

    for (int y=0; y<fb.get_height(); ++y) {
        for (int x=0; x<fb.get_width(); ++x) {
            color c(0.0, 0.0, 0.0);

            for (int p=0; p<rpp_NSquare; ++p) {
                for (int q=0; q<rpp_NSquare; ++q) {
                    float tmin = 1.0;
                    float tmax = std::numeric_limits<float>::infinity();
                    
                    float pOffset = (p + randomOffset())/ rpp_NSquare;
                    float qOffset = (q + randomOffset())/ rpp_NSquare;

                    ray r;
                    cam.generateRay(x + pOffset, y + qOffset, r);
                    c += world.computeRayColor(r, tmin, tmax, recursionDepth);

                }
            }

            c /= (rpp_NSquare * rpp_NSquare);
            fb.setPixelColor(x, y, c);
        }
    }

    fb.exportToPNG("Testing_antialiasing");

    world.clear();
    shared_ptr<Shader> catShader = make_shared<Lambertian>(color(245.0/255.0, 147.0/255.0, 227.0/255.0));
    shared_ptr<Shader> eyeShader = make_shared<BlinnPhong>(color(0.0, 0.0, 0.0));;
    
    // ===== Body =====
    shared_ptr<Sphere> body = make_shared<Sphere>(point3(0.0, 0.0, -10.0), 1.0, catShader);
    world.add(body);

    // ===== Head =====
    shared_ptr<Sphere> head = make_shared<Sphere>(point3(0.0, 1.2, -10.0), 0.6, catShader);
    world.add(head);

    // ===== Ears =====
    shared_ptr<Triangle> leftEar = make_shared<Triangle>(
        point3(-0.5, 1.25, -10.0),   // left base corner
        point3(-0.2, 1.7, -10.0),   // right base corner
        point3(-0.45, 2.1, -10.0),  // tip pointing up
        catShader
    );
    world.add(leftEar);

    shared_ptr<Triangle> rightEar = make_shared<Triangle>(
        point3(0.2, 1.7, -10.0),    // left base corner
        point3(0.5, 1.25, -10.0),    // right base corner
        point3(0.45, 2.1, -10.0),   // tip pointing up
        catShader
    );
    world.add(rightEar);

    // ===== Eyes =====
    shared_ptr<Sphere> leftEye = make_shared<Sphere>(point3(-0.2, 1.3, -9.5), 0.1, eyeShader);
    world.add(leftEye);

    shared_ptr<Sphere> rightEye = make_shared<Sphere>(point3(0.2, 1.3, -9.5), 0.1, eyeShader);
    world.add(rightEye);

    // ===== Nose =====
    shared_ptr<Sphere> nose = make_shared<Sphere>(point3(0.0, 1.1, -9.45), 0.05, catShader);
    world.add(nose);

    // ===== Legs =====
    shared_ptr<Sphere> frontLeftLeg = make_shared<Sphere>(point3(-0.4, -0.7, -9.6), 0.3, catShader);
    world.add(frontLeftLeg);

    shared_ptr<Sphere> frontRightLeg = make_shared<Sphere>(point3(0.4, -0.7, -9.6), 0.3, catShader);
    world.add(frontRightLeg);

    shared_ptr<Sphere> backLeftLeg = make_shared<Sphere>(point3(-0.4, -0.7, -10.4), 0.3, catShader);
    world.add(backLeftLeg);

    shared_ptr<Sphere> backRightLeg = make_shared<Sphere>(point3(0.4, -0.7, -10.4), 0.3, catShader);
    world.add(backRightLeg);    

    // ===== Optional Whiskers =====
    shared_ptr<Triangle> leftWhisker = make_shared<Triangle>(
        point3(-0.05, 1.1, -9.45),
        point3(-0.6, 1.1, -9.45),
        point3(-0.05, 1.12, -9.45),
        catShader
    );
    world.add(leftWhisker);

    shared_ptr<Triangle> rightWhisker = make_shared<Triangle>(
        point3(0.05, 1.1, -9.45),
        point3(0.6, 1.1, -9.45),
        point3(0.05, 1.12, -9.45),
        catShader
    );
    world.add(rightWhisker);

    for (int y=0; y<fb.get_height(); ++y) {
        for (int x=0; x<fb.get_width(); ++x) {
            color c(0.0, 0.0, 0.0);

            for (int p=0; p<rpp_NSquare; ++p) {
                for (int q=0; q<rpp_NSquare; ++q) {
                    float tmin = 1.0;
                    float tmax = std::numeric_limits<float>::infinity();
                    
                    float pOffset = (p + randomOffset())/ rpp_NSquare;
                    float qOffset = (q + randomOffset())/ rpp_NSquare;

                    ray r;
                    cam.generateRay(x + pOffset, y + qOffset, r);
                    c += world.computeRayColor(r, tmin, tmax, recursionDepth);

                }
            }

            c /= (rpp_NSquare * rpp_NSquare);
            fb.setPixelColor(x, y, c);
        }
    }

    fb.exportToPNG("Testing_cat");

    
    return 0;
}
