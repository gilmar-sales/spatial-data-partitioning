#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/particle.hpp"
#include "core/application.hpp"

Application *Application::create_application() {
    return new Application({"Collisions", 1280, 720});;
}

int main(int argc, char const *argv[]) {
    Application::get();

    unsigned circleVAO = initCircle({0.0f, 0.0f}, 10.f);

    auto* particles = new Particle[particles_count];
    init_particles(particles);

    Application::get()->register_system([particles]() {
        for(unsigned i = 0; i < particles_count - 1; i++) {
            Particle& particle = particles[i];

            for(unsigned j = i + 1; j < particles_count; j++)
            {
                Particle& other = particles[j];
                if (particle.intersect(other))
                {
                    glm::vec2 distance = particle.position - other.position;
                    float magnitude = glm::length(distance);


                    glm::vec2 normal = glm::normalize(glm::vec2(other.position.x-particle.position.x, other.position.y-particle.position.y));

                    // apply force
                    float kx = (particle.velocity.x - other.velocity.x);
                    float ky = (particle.velocity.y - other.velocity.y);
                    float p = 2.0f * (normal.x * kx + normal.y * ky) / (particle.radius + other.radius);
                    particle.velocity.x = particle.velocity.x - p * other.radius * normal.x;
                    particle.velocity.y = particle.velocity.y - p * other.radius * normal.y;
                    other.velocity.x = other.velocity.x + p * particle.radius * normal.x;
                    other.velocity.y = other.velocity.y + p * particle.radius * normal.y;

                    // remove intersection
                    glm::vec2 forceDir = distance / magnitude;
                    glm::vec2 force = forceDir;
                    float intersectionLenght = particle.radius + other.radius - glm::length(particle.position - other.position);
                    glm::vec2 correction = force * intersectionLenght;

                    float max_distance = (particle.radius + other.radius);
                    particle.position += correction * particle.radius / max_distance;
                    other.position -= correction * other.radius / max_distance;
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
            particle.position += particle.velocity * Application::delta_time;

            // particle.velocity *= 0.998f;
        }
    });

    Application::get()->register_system([particles, circleVAO](){
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

    return 0;
}
