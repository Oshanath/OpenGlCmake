cmake_minimum_required(VERSION 3.0.0)
project(openglLearn VERSION 0.1.0)

include(CTest)
enable_testing()

#To not compile the glfw tests example and documentation
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

include_directories(include)
add_subdirectory(libraries/glfw-3.3.4)

add_executable(openglLearn
 main.cpp
 src/glad.c
 src/Camera.cpp
 src/EBO.cpp
 src/Helpers.cpp
 src/LightSource.cpp
 src/Mesh.cpp
 src/Model.cpp
 src/shaderClass.cpp
 src/Texture.cpp
 src/VAO.cpp
 src/VBO.cpp
 src/stb.cpp
 )
target_link_libraries(openglLearn glfw)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
