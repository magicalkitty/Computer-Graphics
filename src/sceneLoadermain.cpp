#include <iostream>

#include "PerspectiveCamera.h"
#include "Scene.h"
#include "Sphere.h"
#include "triangle.h"
#include "Lambertian.h"
#include "BlinnPhong.h"
#include "NormalShader.h"
#include "mirrorShader.h"
#include "JSONParser/SceneContainer.h"
#include "JSONParser/SceneLoader.h"
#include "JSONParser/SceneParser_JSON.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <scene.json>\n";
    return 1;
    }

    SceneContainer scene;

    std::shared_ptr<ISceneLoader> loader = std::make_shared<SceneLoader>(scene);
    SceneParser_JSON parser(loader);

    std::string filename = argv[1];
    parser.parseFileData(filename);

    // ------------------------------------------
    // Convert parsed scene into our render world
    // ------------------------------------------

    /* ---- Setup framebuffer ----*/
    int nx = 600;
    int ny = 600;
    Framebuffer fb(nx, ny);

    Scene world;

    fb.clearToColor(color(0.0, 0.0, 0.0));

    /* ---- Set up camera ---- */
    PerspectiveCamera cam(vec3(0, 0, 0), vec3(0, 0, -1), nx, ny, 1.0f);

    // if (!scene.getCameras().empty()) {
    //     auto camJSON = scene.getCameras()[0];
    //     std::cout << "Camera raw string (type only): " << camJSON->type << std::endl;
    // }



}