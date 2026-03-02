add_library (cs4212-util
  ArgumentParsing.cpp ArgumentParsing.h
  handleGraphicsArgs.cpp handleGraphicsArgs.h
  model_obj.cpp model_obj.h
  framebuffer.cpp framebuffer.h
  ray.h
  main.cpp
  Sphere.cpp Sphere.h
  Triangle.cpp Triangle.h
  Camera.cpp Camera.h
  PerspectiveCamera.cpp PerspectiveCamera.h
  Scene.cpp Scene.h
  # RayTracer.cpp RayTracer.h
  Light.h
  Shader.h
  Lambertian.cpp Lambertian.h
  BlinnPhong.cpp BlinnPhong.h
  NormalShader.cpp NormalShader.h
  MirrorShader.cpp MirrorShader.h
  # DiffuseShader.cpp DiffuseShader.h
  # Random.cpp Random.h
  )

target_compile_definitions(cs4212-util PUBLIC HAS_GLM)
target_link_libraries(cs4212-util PRIVATE Boost::program_options)
target_link_libraries(cs4212-util PUBLIC glm::glm)
find_package(PNG REQUIRED)
target_link_libraries(cs4212-util PUBLIC PNG::PNG)

add_executable(main
  main.cpp
)

target_link_libraries(main
    PRIVATE
        cs4212-util
        Boost::program_options
        PNG::PNG
)

# -----------------------------------
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
# Enable OpenMP for all executables
# Must be done **after executables are defined**
find_package(OpenMP REQUIRED)

# Link OpenMP using keyword style
target_link_libraries(main PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(readTriangleVerts PRIVATE OpenMP::OpenMP_CXX)