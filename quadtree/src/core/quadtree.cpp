#include "quadtree.h"

#include <iostream>

QuadTree::QuadTree(glm::vec2 position, float half_range, unsigned capacity)
{
    m_position = position;
    m_half_range = half_range;
    m_elements = std::vector<Particle*>(capacity);
    m_capacity = capacity;

    m_top_left = nullptr;
    m_top_right = nullptr;
    m_bot_left = nullptr;
    m_bot_right = nullptr;
}

bool QuadTree::insert(Particle* element)
{
    if (!contains(element))
    {
        return false;
    }

    if (m_count < m_capacity)
    {
        m_elements[m_count++] = element;
        return true;
    } else if (m_top_left == nullptr) {
        subdivide();
    }

    return (
        m_top_left->insert(element) ||
        m_top_right->insert(element) ||
        m_bot_left->insert(element) ||
        m_bot_right->insert(element)
    );
}

void QuadTree::subdivide()
{
    float half_range = m_half_range / 2;

    glm::vec2 top_left_pos = m_position-half_range; 
    glm::vec2 top_right_pos = {m_position.x+half_range, m_position.y-half_range}; 
    glm::vec2 bot_left_pos = {m_position.x-half_range, m_position.y+half_range}; 
    glm::vec2 bot_right_pos = m_position+half_range; 

    m_top_left = std::make_unique<QuadTree>(top_left_pos, half_range, m_capacity);
    m_top_right = std::make_unique<QuadTree>(top_right_pos, half_range, m_capacity);
    m_bot_left = std::make_unique<QuadTree>(bot_left_pos, half_range, m_capacity);
    m_bot_right = std::make_unique<QuadTree>(bot_right_pos, half_range, m_capacity);
}

void QuadTree::query(Particle* particle, std::vector<Particle*>* found)
{
    if (!intersect(particle))
    {
        return;
    }

    for (unsigned i = 0; i < m_count; i++)
    {
        if(particle == m_elements[i])
            continue;

        if (contains(m_elements[i]))
        {
            found->push_back(m_elements[i]);
        }
    }

    if (m_top_left)
    {
        m_top_left->query(particle, found);
        m_top_right->query(particle, found);
        m_bot_left->query(particle, found);
        m_bot_right->query(particle, found);
    }
}

bool QuadTree::contains(Particle* particle)
{
    return (
        particle->position.x >= m_position.x - m_half_range &&
        particle->position.x <= m_position.x + m_half_range &&
        particle->position.y >= m_position.y - m_half_range &&
        particle->position.y <= m_position.y + m_half_range
    );
}

bool QuadTree::intersect(Particle* particle)
{
    float distance = glm::distance(m_position, particle->position);
    return (
        particle->position.x >= m_position.x - (m_half_range + particle->radius) &&
        particle->position.x <= m_position.x + (m_half_range + particle->radius) &&
        particle->position.y >= m_position.y - (m_half_range + particle->radius) &&
        particle->position.y <= m_position.y + (m_half_range + particle->radius)
    );
}

void QuadTree::draw(unsigned vao, unsigned shaderProgram)
{
    static float color[3] = {1.f,1.f,1.f};
    glm::mat4 model          = glm::mat4(1.0f);
    model       = glm::translate(model, glm::vec3(m_position, 0.0f));
    model       = glm::scale(model, glm::vec3(m_half_range, m_half_range, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &color[0]);
    glBindVertexArray(vao);
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);

    if(m_top_left)
    {
        m_top_left->draw(vao, shaderProgram);
        m_top_right->draw(vao, shaderProgram);
        m_bot_left->draw(vao, shaderProgram);
        m_bot_right->draw(vao, shaderProgram);
    }
}