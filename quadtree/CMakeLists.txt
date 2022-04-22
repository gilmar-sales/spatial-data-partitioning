cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)


find_package(OpenGL REQUIRED)

file(GLOB_RECURSE COMMON_SOURCES src/core/*.cpp)

add_executable(collisions
    ${COMMON_SOURCES}
    "src/collisions.cpp"
)

target_link_libraries(collisions
    glm
    glfw
    glad
)

target_precompile_headers(collisions
PUBLIC
        src/core/common.hpp
)

add_executable(collisions_quadtree
    ${COMMON_SOURCES}
    "src/collisions_quadtree.cpp"
)

target_link_libraries(collisions_quadtree
    glm
    glfw
    glad
)

target_precompile_headers(collisions_quadtree
PUBLIC
        src/core/common.hpp
)

add_executable(collisions_quadtree_threads
    ${COMMON_SOURCES}
    "src/collisions_quadtree_threads.cpp"
)

target_link_libraries(collisions_quadtree_threads
    glm
    glfw
    glad
)

target_precompile_headers(collisions_quadtree_threads
PUBLIC
        src/core/common.hpp
)
