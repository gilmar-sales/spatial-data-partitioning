cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)


find_package(OpenGL REQUIRED)

add_executable(collisions
    "src/collisions.cpp"
)

target_link_libraries(collisions
    glm
    glfw
    glad
)

target_precompile_headers(collisions
PUBLIC
    src/common.h 
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

target_precompile_headers(collisions_quadtree
PUBLIC
    src/common.h 
)

add_executable(collisions_quadtree_threads
    "src/collisions_quadtree_threads.cpp"
    "src/quadtree.cpp"
    "src/quadtree.h"
    "src/particle.h"
)

target_link_libraries(collisions_quadtree_threads
    glm
    glfw
    glad
)

target_precompile_headers(collisions_quadtree_threads
PUBLIC
    src/common.h 
)
