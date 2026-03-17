# CMAKE_MINIMUM_REQUIRED (VERSION 3.22)

# project(sceneLoaderExample
#   VERSION 1.0
#   LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# Include(FetchContent)
# FetchContent_Declare(
#   Catch2
#   GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#   GIT_TAG        v3.8.0 # or a later release
# )
# FetchContent_MakeAvailable(Catch2)

# find_package(glm CONFIG REQUIRED)
# find_package(nlohmann_json CONFIG REQUIRED)

# add_executable(sceneLoaderTest
# 	ISceneLoader.h SceneLoader.h
# 	SceneContainer.h
# 	SceneParser_JSON.cpp SceneParser_JSON.h
# 	JSONmain.cpp
# )
# target_link_libraries(sceneLoaderTest PRIVATE glm::glm-header-only)
# target_link_libraries(sceneLoaderTest PRIVATE nlohmann_json::nlohmann_json)

# enable_testing()
# include(CTest)
# include(Catch)

# add_executable(test_sceneParsing_JSON
# 	SceneParser_JSON.cpp SceneParser_JSON.h
# 	test_sceneParsing_JSON.cpp)
# target_link_libraries(test_sceneParsing_JSON PRIVATE Catch2::Catch2WithMain)
# target_link_libraries(test_sceneParsing_JSON PRIVATE nlohmann_json::nlohmann_json)
# catch_discover_tests(test_sceneParsing_JSON)
