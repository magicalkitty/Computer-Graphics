Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.0 # or a later release
)
FetchContent_MakeAvailable(Catch2)

find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(PNG REQUIRED)          # ⚠️ ADDED here once
find_package(Boost REQUIRED COMPONENTS program_options)  # ⚠️ ADDED explicitly for linking

# -------------------------
add_library (cs4212-util
  ArgumentParsing.cpp ArgumentParsing.h
  handleGraphicsArgs.cpp handleGraphicsArgs.h
  model_obj.cpp model_obj.h
  framebuffer.cpp framebuffer.h
  ray.h
  Sphere.cpp Sphere.h
  Triangle.cpp Triangle.h
  Camera.cpp Camera.h
  PerspectiveCamera.cpp PerspectiveCamera.h
  Scene.cpp Scene.h
  Light.h
  Shader.h
  Lambertian.cpp Lambertian.h
  BlinnPhong.cpp BlinnPhong.h
  NormalShader.cpp NormalShader.h
  MirrorShader.cpp MirrorShader.h
  OBJMesh.cpp OBJMesh.h
  BoundingBox.h
  BoundingVolumeNode.cpp BoundingVolumeNode.h
)

target_compile_definitions(cs4212-util PUBLIC HAS_GLM)
target_link_libraries(cs4212-util
  PRIVATE Boost::program_options
  PUBLIC glm::glm PNG::PNG
)

# -------------------------
# Main executable
add_executable(main
  main.cpp
)

target_link_libraries(main
  PRIVATE
    cs4212-util
    Boost::program_options
    PNG::PNG
)

# -------------------------
# triangleChallenge executable
add_executable(readTriangleVerts
  triangleChallenge/readTriangleVerts.cpp
)

target_link_libraries(readTriangleVerts
  PRIVATE
    cs4212-util
    Boost::program_options
    PNG::PNG
)

# -------------------------
# JSON Parser executable
add_executable(sceneLoaderTest
  JSONParser/SceneParser_JSON.cpp
  JSONParser/sceneLoaderTest.cpp
  # ⚠️ REMOVE header files here — headers are not compiled
)

target_link_libraries(sceneLoaderTest
  PRIVATE
    cs4212-util
    glm::glm-header-only
    nlohmann_json::nlohmann_json
    Boost::program_options
    PNG::PNG
)

# -------------------------
# Enable OpenMP for all executables
find_package(OpenMP REQUIRED)

target_link_libraries(main PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(readTriangleVerts PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(sceneLoaderTest PRIVATE OpenMP::OpenMP_CXX)

# -------------------------
# Catch2 test executable
enable_testing()
include(CTest)
include(Catch)

add_executable(test_sceneParsing_JSON
  JSONParser/SceneParser_JSON.cpp
  JSONParser/test_sceneParsing_JSON.cpp
)
target_link_libraries(test_sceneParsing_JSON
  PRIVATE
    Catch2::Catch2WithMain
    nlohmann_json::nlohmann_json
)
catch_discover_tests(test_sceneParsing_JSON)