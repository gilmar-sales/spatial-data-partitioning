cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)

project(Quadtree)

find_package(OpenGL REQUIRED)

add_executable(collisions
    "src/collisions.cpp"
)

target_link_libraries(collisions
    glm
    glfw
    glad
)

add_executable(collisions_quadtree
    "src/collisions_quadtree.cpp"
    "src/quadtree.cpp"
    "src/quadtree.h"
    "src/particle.h"
)

target_link_libraries(collisions_quadtree
    glm
    glfw
    glad
)