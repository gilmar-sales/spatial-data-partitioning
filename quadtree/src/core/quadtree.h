#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "particle.hpp"

class QuadTree {
public:
    QuadTree(glm::vec2 position, float half_range, unsigned capacity);
    ~QuadTree() = default;

    bool insert(Particle* particle);
    void subdivide();
    void query(Particle* particle, std::vector<Particle*>* found);
    bool contains(Particle* particle);
    bool intersect(Particle* particle);
    void draw(unsigned vao, unsigned shaderProgram);

private:
    glm::vec2 m_position;
    float m_half_range;
    std::vector<Particle*> m_elements;
    unsigned m_capacity;
    unsigned m_count = 0;

    std::unique_ptr<QuadTree> m_top_left;
    std::unique_ptr<QuadTree> m_top_right;
    std::unique_ptr<QuadTree> m_bot_left;
    std::unique_ptr<QuadTree> m_bot_right;
};