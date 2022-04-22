#pragma once

#include <random>
#include <glad/glad.h>
#include "particle.hpp"
#include "application.hpp"

const unsigned particles_count = 15000;
const int max_radius = 3;
const int min_radius = 3;

void init_particles(Particle* particles);
unsigned initCircle(glm::vec2 point, float radius);
unsigned initQuad(glm::vec2 point);