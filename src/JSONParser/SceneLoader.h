#pragma once

#include "ISceneLoader.h"
#include "SceneContainer.h"
#include <iostream>
#include <unordered_map>
#include "../PerspectiveCamera.h"
#include "../Sphere.h"
#include "../triangle.h"
#include "../Shader.h"
#include "../Lambertian.h"
#include "../BlinnPhong.h"
#include "../NormalShader.h"
#include "../mirrorShader.h"
#include "../../renderlib/vec3.h"
#include "../light.h"
#include "../Framebuffer.h"

std::unordered_map<std::string, std::shared_ptr<Shader>> shaderMap;

class SceneLoader : public ISceneLoader {
private:
  SceneContainer &m_targetScene; // Reference to the external scene

public:
  // The caller provides the scene to be filled
  SceneLoader(SceneContainer &sceneToPopulate, int nx, int ny)
      : m_targetScene(sceneToPopulate), numShaders(0), numTextures(0), nx(nx), ny(ny) {}

  void reserveCameras(size_t count) override {
    // makes sure the vector has enough space
    m_targetScene.cameras.reserve(count);
  }
  void reserveLights(size_t count) override {
    m_targetScene.lights.reserve(count);
  }
  void reserveShapes(size_t count) override {
    m_targetScene.shapes.reserve(count);
  }
  void reserveShaders(size_t count) override { numShaders = count; }
  void reserveTextures(size_t count) override { numTextures = count; }

  void setSceneParams(const SceneParams &sparams) override {
    std::cout << "Setting scene params." << std::endl;
  }

  void addCamera(const std::string &name, const std::string &type,
                 const ISceneLoader::vec &pos, const ISceneLoader::vec &viewDir,
                 float focalLength, float imagePlaneWidth) override {
    std::cout << "Creating camera: " << name << ", type:" << type << std::endl;
    vec3 position(pos.x, pos.y, pos.z);
    vec3 direction(viewDir.x, viewDir.y, viewDir.z);
    
    if (type == "perspective") {
      auto cam = std::make_shared<PerspectiveCamera>(position, direction, nx, ny, focalLength);
      m_targetScene.cameras.push_back(cam);
    }
    
    else {
      auto cam = std::make_shared<PerspectiveCamera>(position, direction, nx, ny, focalLength);
      m_targetScene.cameras.push_back(cam);
    }
  }
  void addPointLight(const ISceneLoader::vec &pos,
                     const ISceneLoader::vec &intensity) override {
    std::cout << "Creating point light." << std::endl;

    vec3 position(pos.x, pos.y, pos.z);
    vec3 color(intensity.x, intensity.y, intensity.z);

    auto light = std::make_shared<Light>(position, color);

    m_targetScene.lights.push_back(light);
  }
  void addAreaLight(const ISceneLoader::vec &pos,
                    const ISceneLoader::vec &color,
                    const ISceneLoader::vec &normal, float width,
                    float length) override {
    std::cout << "Creating area light." << std::endl;
  }
  void addShader(const ISceneLoader::ShaderDesc &shaderDesc) override {
    std::cout << "Creating shader: type=" << shaderDesc.type << std::endl;
    // I place my shaders on an unordered map often
    std::shared_ptr<Shader> shaderPtr;

    if (shaderDesc.type == "Lambertian") {
      vec3 diffuse(shaderDesc.diffuse.data.x, shaderDesc.diffuse.data.y, shaderDesc.diffuse.data.z);
      shaderPtr = std::make_shared<Lambertian>(diffuse);
    }

    else if (shaderDesc.type == "BlinnPhong") {
      vec3 diffuse(shaderDesc.diffuse.data.x, shaderDesc.diffuse.data.y, shaderDesc.diffuse.data.z);
      vec3 specular(shaderDesc.specular.data.x, shaderDesc.specular.data.y, shaderDesc.specular.data.z);
      shaderPtr = std::make_shared<BlinnPhong>(diffuse, specular, shaderDesc.phongExp);
    }

    else if (shaderDesc.type == "NormalShader") {
      shaderPtr = std::make_shared<NormalShader>();
    }

    else if (shaderDesc.type == "Mirror") {
      shaderPtr = std::make_shared<MirrorShader>();
    }

    else {
      shaderPtr = std::make_shared<NormalShader>();
    }

    shaderMap[shaderDesc.name] = shaderPtr;

  }
  void addShape(const ISceneLoader::ShapeDesc &shapeDesc) override {
    std::cout << "Creating shape: type=" << shapeDesc.type << std::endl;

    std::shared_ptr<Shader> shader = nullptr;
    if (!shapeDesc.shaderNameReference.empty()) {
      std::cout << "shaderNameReference='" << shapeDesc.shaderNameReference << "'" << std::endl;
      shader = shaderMap[shapeDesc.shaderNameReference];
      printf("Found shader reference! %s\n", shapeDesc.shaderNameReference.c_str());
    }
    if (shapeDesc.type == "sphere") {
      vec3 center(shapeDesc.center.x, shapeDesc.center.y, shapeDesc.center.z);

      auto sphere = std::make_shared<Sphere>(center, shapeDesc.radius, shader);

      m_targetScene.shapes.push_back(sphere);
    }

    else if (shapeDesc.type == "triangle") {
      vec3 a (shapeDesc.v0.x, shapeDesc.v0.y, shapeDesc.v0.z);
      vec3 b (shapeDesc.v1.x, shapeDesc.v1.y, shapeDesc.v1.z);
      vec3 c (shapeDesc.v2.x, shapeDesc.v2.y, shapeDesc.v2.z);

      auto tri = std::make_shared<Triangle>(a, b, c, shader);

      m_targetScene.shapes.push_back(tri);
    }
  }
  void addTexture(const std::string &type, const std::string &name,
                  const std::string &sourceFile) override {
    std::cout << "Creating texture: type=" << type << ", name=" << name
              << ", sourceFile=" << sourceFile << std::endl;
  }

public:
  int numShaders;
  int numTextures;
  int nx, ny;
};