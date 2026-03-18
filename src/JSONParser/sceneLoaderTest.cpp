#include <iostream>

#include "../PerspectiveCamera.h"
#include "../Scene.h"
#include "../Sphere.h"
#include "../triangle.h"
#include "../Lambertian.h"
#include "../BlinnPhong.h"
#include "../NormalShader.h"
#include "../mirrorShader.h"
#include "../handleGraphicsArgs.h"

#include "SceneContainer.h"
#include "SceneLoader.h"
#include "SceneParser_JSON.h"

#include <random>

#ifdef _OPENMP
#include <omp.h>
#endif

float randomOffset() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <scene.json>\n";
    return 1;
  }

  // ------------------------------------------
  // Graphics args
  // ------------------------------------------

  sivelab::GraphicsArgs args;
  args.process(argc, argv);

  int nx = (args.width > 0) ? args.width : 100;
  int ny = (args.height > 0) ? args.height : 100;

  Framebuffer fb(nx, ny);

  
  int rpp_NSquare = (args.rpp > 0) ? args.rpp : 4;
  int recursionDepth = (args.recursionDepth > 0) ? args.recursionDepth : 10;

  // ------------------------------------------
  // Load JSON scene
  // ------------------------------------------

  SceneContainer scene;

  std::shared_ptr<ISceneLoader> loader = std::make_shared<SceneLoader>(scene, nx, ny);
  SceneParser_JSON parser(loader);

  std::string filename = argv[1];
  parser.parseFileData(filename);

  // ------------------------------------------
  // Convert parsed scene into our render world
  // ------------------------------------------
  
  Scene world;

  fb.clearToColor(color(1.0, 1.0, 1.0));
  
  /* ---- Set up camera ---- */
  auto cameraPtr = scene.getCameras()[0];
  PerspectiveCamera *cam = dynamic_cast<PerspectiveCamera*>(cameraPtr.get());
  if (!cam) {
    printf("Camera type not recognized or not supported. Defaulting to perspective camera.\n");
    cam = new PerspectiveCamera(vec3(0, 0, 0), vec3(0, 0, -1), nx, ny, 1.0f);
  }

  /* ---- Add objects ---- */
  for (auto &shape : scene.getShapes()) {
    world.add(shape);
  }

  for (auto &light : scene.getLights()) {
    world.addLight(light);
  }

  /* ---- Rendering Loop ---- */
  #pragma omp parallel for schedule(dynamic)
  for (int y=0; y<fb.get_height(); ++y) {
    printf("Still rendering... %d%%\n", (y * 100) / fb.get_height());
    for (int x=0; x<fb.get_width(); ++x) {
        color c(0.0, 0.0, 0.0);

        for (int p=0; p<rpp_NSquare; ++p) {
            for (int q=0; q<rpp_NSquare; ++q) {
                float tmin = 1.0;
                float tmax = std::numeric_limits<float>::infinity();
                
                float pOffset = (p + randomOffset())/ rpp_NSquare;
                float qOffset = (q + randomOffset())/ rpp_NSquare;

                ray r;
                cam->generateRay(x + pOffset, y + qOffset, r);
                c += world.computeRayColor(r, tmin, tmax, recursionDepth);
                // cam->generateRay(x, y, r);
                // color c = world.computeRayColor(r, tmin, tmax, light, recursionDepth);

            }
        }

        c /= (rpp_NSquare * rpp_NSquare);
        fb.setPixelColor(x, y, c);
      }
    }

  fb.exportToPNG("sceneLoaderTest_output");
  return 0;
}