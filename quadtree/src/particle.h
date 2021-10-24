
#pragma once
#include <glm/glm.hpp>

#include <iostream>

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    float radius;
    glm::vec3 color;
};