add_library (cs4212-util
  ArgumentParsing.cpp ArgumentParsing.h
  handleGraphicsArgs.cpp handleGraphicsArgs.h
  model_obj.cpp model_obj.h
  framebuffer.cpp framebuffer.h
  ray.h
  main.cpp
)
target_compile_definitions(cs4212-util PUBLIC HAS_GLM)
target_link_libraries(cs4212-util PRIVATE Boost::program_options)
target_link_libraries(cs4212-util PUBLIC glm::glm)
find_package(PNG REQUIRED)
target_link_libraries(cs4212-util PUBLIC PNG::PNG)

add_executable(main
  main.cpp
)
target_link_libraries(main cs4212-util)
target_link_libraries(main ${Boost_PROGRAM_OPTIONS_LIBRARIES})
target_link_libraries(main PNG::PNG)