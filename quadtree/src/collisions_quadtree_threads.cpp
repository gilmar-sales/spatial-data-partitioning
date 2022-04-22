#include <vector>
#include <thread>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/particle.hpp"
#include "core/application.hpp"
#include "core/quadtree.h"

Application *Application::create_application() {
    return new Application({"Collisions - Quadtree - Threads", 1280, 720});
}

void update_physics(Particle* particles, const std::shared_ptr<QuadTree>& quad_tree, unsigned count, float deltaTime);

int main(int argc, char const *argv[]) {
    Application::get();

    unsigned circleVAO = initCircle({0.0f, 0.0f}, 10.f);
    unsigned quadVAO = initQuad({0.0f, 0.0f});

    auto* particles = new Particle[particles_count];
    init_particles(particles);

    glm::vec2 screen_center = {0,0};

    unsigned thread_count = std::thread::hardware_concurrency();
    auto* threads = new std::thread[thread_count];

    Application::get()->register_system([particles, screen_center, quadVAO, threads, thread_count](){
        float half_width = (float) Application::get()->get_window().get_width() / 2.f;
        //create quadtree
        std::shared_ptr<QuadTree> quad_tree = std::make_shared<QuadTree>(screen_center, half_width, 6);

        for (unsigned i = 0; i < particles_count; i++) {
            quad_tree->insert(&particles[i]);
        }

        //quad_tree->draw(quadVAO, Application::get()->get_shader_program());

        uint thread_load = particles_count / thread_count;

        //update physics
        for (uint i = 0; i < thread_count; i++) {
            threads[i] = std::thread(update_physics, &particles[thread_load*i], quad_tree, thread_load, Application::delta_time);
        }

        for (uint i = 0; i < thread_count; i++) {
            threads[i].join();
        }

    });

    Application::get()->register_system([circleVAO, particles]() {
        //render particles
        for (unsigned i = 0; i < particles_count; i++)
        {
            glm::mat4 model          = glm::mat4(1.0f);
            model       = glm::translate(model, glm::vec3(particles[i].position, 0.0f));
            model       = glm::scale(model, glm::vec3(particles[i].radius, particles[i].radius, 0.0f));

            glUniformMatrix4fv(glGetUniformLocation(Application::get()->get_shader_program(), "model"), 1, GL_FALSE, &model[0][0]);
            glUniform3fv(glGetUniformLocation(Application::get()->get_shader_program(), "color"), 1, &particles[i].color[0]);
            glBindVertexArray(circleVAO);
            glDrawElements(GL_TRIANGLES, 90, GL_UNSIGNED_INT, nullptr);
        }
    });

    Application::get()->run();

    glDeleteVertexArrays(1, &circleVAO);
    glDeleteVertexArrays(1, &quadVAO);

    return 0;
}

void update_physics(Particle* particles, const std::shared_ptr<QuadTree>& quad_tree, unsigned count, float deltaTime)
{
    for (unsigned i = 0; i < count; i++)
    {
        Particle& particle = particles[i];
        std::vector<Particle*> elements = {};

        quad_tree->query(&particle, &elements);

        for(Particle* other : elements)
        {
            if (particle.intersect(*other))
            {
                glm::vec2 distance = particle.position - other->position;
                float magnitude = glm::length(distance);

                glm::vec2 normal = glm::normalize(glm::vec2(other->position.x-particle.position.x, other->position.y-particle.position.y));

                // apply force
                float kx = (particle.velocity.x - other->velocity.x);
                float ky = (particle.velocity.y - other->velocity.y);
                float p = 2.0f * (normal.x * kx + normal.y * ky) / (particle.radius + other->radius);
                particle.velocity.x = particle.velocity.x - p * other->radius * normal.x;
                particle.velocity.y = particle.velocity.y - p * other->radius * normal.y;
                other->velocity.x = other->velocity.x + p * particle.radius * normal.x;
                other->velocity.y = other->velocity.y + p * particle.radius * normal.y;

                // remove intersection
                glm::vec2 forceDir = distance / magnitude;
                glm::vec2 force = forceDir;
                float intersectionLenght = particle.radius + other->radius - glm::length(particle.position - other->position);
                glm::vec2 correction = force * intersectionLenght;

                float max_distance = (particle.radius + other->radius);
                particle.position += correction * particle.radius / max_distance;
                other->position -= correction * other->radius / max_distance;
            }
        }

        float half_width = (float) Application::get()->get_window().get_width() / 2.f;
        float half_height = (float) Application::get()->get_window().get_height() / 2.f;

        float maxX = half_width - particle.radius;
        float minX = -maxX;

        float maxY = half_height - particle.radius;
        float minY = -maxY;

        // screen bounds
        if (particle.position.x <= minX)
        {
            particle.velocity.x = -particle.velocity.x;
            particle.position.x = minX;
        } else if (particle.position.x >= maxX ) {
            particle.velocity.x = -particle.velocity.x;
            particle.position.x = maxX;
        }

        if (particle.position.y <= minY)
        {
            particle.velocity.y = -particle.velocity.y;
            particle.position.y = minY;
        } else if (particle.position.y >= maxY)
        {
            particle.velocity.y = -particle.velocity.y;
            particle.position.y = maxY;
        }

        //update physic values
        particle.position += particle.velocity * deltaTime;

        // particle.velocity *= 0.998f;
    }
}