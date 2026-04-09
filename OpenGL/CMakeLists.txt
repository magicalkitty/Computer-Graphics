include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable (glfwExample
  glfwExample.cpp 
  GLSL.cpp GLSL.h
  ../src/Camera.cpp ../src/Camera.h
  ../src/PerspectiveCamera.cpp ../src/PerspectiveCamera.h
  ../src/OBJMesh.cpp ../src/OBJMesh.h
  ../src/model_obj.cpp ../src/model_obj.h
  ../src/BlinnPhong.cpp ../src/BlinnPhong.h
  ../src/Shader.h
  ../src/Lambertian.cpp ../src/Lambertian.h
  ../src/NormalShader.cpp ../src/NormalShader.h
  ../src/Ray.h
  ../src/Scene.cpp ../src/Scene.h
  ../src/BoundingVolumeNode.cpp ../src/BoundingVolumeNode.h
  ../src/BoundingBox.h
  ../src/Triangle.cpp ../src/Triangle.h
)

target_link_libraries (glfwExample PRIVATE GLEW::GLEW)
target_link_libraries (glfwExample PRIVATE glfw)

if(APPLE)
    target_link_libraries(glfwExample PRIVATE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
else()
    target_link_libraries(glfwExample PRIVATE OpenGL::GL)
endif()

target_link_libraries (glfwExample PRIVATE PNG::PNG)
target_link_libraries (glfwExample PRIVATE ZLIB::ZLIB)

