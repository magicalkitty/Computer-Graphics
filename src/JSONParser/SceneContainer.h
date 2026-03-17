#pragma once

#include <memory>
#include <vector>
#include <string>
#include "../../renderlib/vec3.h"

// class Light {
// public:
//   std::string type;
//   vec3 position;
//   vec3 intensity;
// };

// class Shape {
// public:
//   std::string type;
//   vec3 center;
//   float radius;
//   vec3 a, b, c;
//   std::string shaderRef;
// };

// class Camera {
// public:
//   std::string type;
//   vec3 position;
//   vec3 viewDir;
// };

class SceneContainer {
  friend class SceneLoader; // this will allow the scene loader to
                            // manipulate the Scene

private:
  std::vector<std::shared_ptr<Light>> lights;
  std::vector<std::shared_ptr<hittable>> shapes;
  std::vector<std::shared_ptr<Camera>> cameras;

public:
  SceneContainer() = default;

  const std::vector<std::shared_ptr<Camera>>& getCameras() const {
      return cameras;
  }
  const std::vector<std::shared_ptr<Light>>& getLights() const {
      return lights;
  }
  const std::vector<std::shared_ptr<hittable>>& getShapes() const {
      return shapes;
  }
};