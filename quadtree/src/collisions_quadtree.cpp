#include <iostream>
#include <vector>
#include <random>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaders.h"
#include "quadtree.h"
#include "particle.h"

std::string window_title = "Collisions with QuadTree";

const uint particles_count = 10000;
float max_radius = 3.f;
float min_radius = 3.f;

float screen_width = 1280.f;
float screen_height = 720.f;
float half_width = screen_width / 2.f;
float half_height = screen_height / 2.f;

uint initCircle(glm::vec2 point);
uint initQuad(glm::vec2 point);
bool intersect(Particle& particle, Particle& other);

void update_physics(Particle* particles, std::shared_ptr<QuadTree> quad_tree, uint count, float deltaTime);

int main(int argc, char const *argv[])
{
    uint glfwInitialized = glfwInit();

    assert(glfwInitialized);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, window_title.c_str(), nullptr, nullptr);

    glfwMakeContextCurrent(window);

    uint gladInitialized = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    assert(gladInitialized);

    glViewport(0, 0, screen_width, screen_height);
    glEnable(GL_MULTISAMPLE); 

    std::cout << glfwGetVersionString() << std::endl;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //Initialize shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    checkShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    checkShader(fragmentShader);

    static uint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);

    int success;
    char infoLog[512];
// check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);  

    glm::mat4 projection = glm::mat4(1.0f);

    projection = glm::ortho(-half_width, half_width, -half_height, half_height,-1000.0f, 1000.0f);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);


    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        screen_width = width;
        screen_height = height;

        half_width = width / 2.f;
        half_height = height / 2.f;
        
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::ortho(-half_width, half_width, -half_height, half_height,-1000.0f, 1000.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        glViewport(0, 0, width, height);
    });


    uint circleVAO = initCircle({0.0f, 0.0f});
    uint quadVAO = initQuad({0.0f, 0.0f});

    Particle particles[particles_count] = {};

    std::default_random_engine generator;
    std::uniform_int_distribution<int> x_distribution(-(half_width-max_radius),half_width-max_radius);
    std::uniform_int_distribution<int> y_distribution(-(half_height-max_radius), half_height-max_radius);
    std::uniform_int_distribution<int> velocity_distribution(10, 20);
    std::uniform_int_distribution<int> color_distribution(25, 100);
    std::uniform_int_distribution<int> radius_distribution(min_radius, max_radius);

    for (uint i = 0; i < particles_count; i++) 
    {
        Particle& particle = particles[i];
        particle.position.x = x_distribution(generator);
        particle.position.y = y_distribution(generator);

        particle.velocity = (particle.position / glm::length(particle.position)) * (float) velocity_distribution(generator);

        particle.radius = radius_distribution(generator);

        particle.color.x = (float) color_distribution(generator) / 100;
        particle.color.y = (float) color_distribution(generator) / 100;
        particle.color.z = (float) color_distribution(generator) / 100;
    }

    double lastTime = glfwGetTime();
    double frameTimeAccumulator = 0;
    double framesPerSecond = 0;
    float deltaTime = 0;

    glfwSwapInterval(0);

    glm::vec2 screen_center = {0,0};

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        //create quadtree
        std::shared_ptr<QuadTree> quad_tree = std::make_shared<QuadTree>(screen_center, half_width, 5);

        for (Particle& particle : particles)
        {
            quad_tree->insert(&particle);
        }

        quad_tree->draw(quadVAO, shaderProgram);

        //update physics
        update_physics(particles, quad_tree, particles_count, deltaTime);

        //render particles
        for (uint i = 0; i < particles_count; i++)
        {
            glm::mat4 model          = glm::mat4(1.0f);
            model       = glm::translate(model, glm::vec3(particles[i].position, 0.0f));
            model       = glm::scale(model, glm::vec3(particles[i].radius, particles[i].radius, 0.0f));

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &particles[i].color[0]);
            glBindVertexArray(circleVAO);
            glDrawElements(GL_TRIANGLES, 90, GL_UNSIGNED_INT, 0);
        }


        glfwSwapBuffers(window);

        glfwPollEvents();    

        double now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        frameTimeAccumulator += deltaTime;
        framesPerSecond++;
        if (frameTimeAccumulator >= 1)
        {
            std::stringstream fmt;

            fmt << window_title << " fps: " << framesPerSecond;

            glfwSetWindowTitle(window, fmt.str().c_str());
            frameTimeAccumulator = 0;
            framesPerSecond = 0;
        }
    }

    glDeleteVertexArrays(1, &circleVAO);
    glDeleteVertexArrays(1, &quadVAO);

    glfwTerminate();

    return 0;
}

uint initCircle(glm::vec2 point) {
    std::vector<glm::vec2> vertices = std::vector<glm::vec2>(32);

    vertices[0] = point;


    for (uint i = 1; i < 32; i++)
    {
        float angle = (360.f/30.f) * (i - 1);
        glm::vec2 vertice = {glm::cos(glm::radians(angle)), glm::sin(glm::radians(angle))};
        vertices[i] = vertice;
    }

    std::vector<uint> indices = std::vector<uint>(96);
    

    for(uint i = 0; i < 32; i++) {
        indices[0+(i*3)] = 0;
        indices[1+(i*3)] = i+1;
        indices[2+(i*3)] = i+2;
    }

    //indices[89] = 1;
    
     uint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])* vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
    
    return VAO;
}


uint initQuad(glm::vec2 point) {
    
    std::vector<glm::vec2> vertices = {
        {-1,1},
        {1,1},
        {1, -1},
        {-1,-1}
    };


    std::vector<uint> indices = {
        0, 1,
        1, 2,
        2,3,
        3,0
    };

    
     uint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])* vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
    
    return VAO;
}

bool intersect(Particle& particle, Particle& other)
{
    return glm::distance(particle.position, other.position) <= particle.radius + other.radius;
}

void update_physics(Particle* particles, std::shared_ptr<QuadTree> quad_tree, uint count, float deltaTime)
{
    for (uint i = 0; i < count; i++)
        {
            Particle& particle = particles[i];
            std::vector<Particle*> elements = {};

            quad_tree->query(&particle, &elements);

            for(Particle* other : elements)
            {
                if (intersect(particle, *other))
                {
                    glm::vec2 distance = particle.position - other->position;
                    float magnitude = glm::length(distance);

                    glm::vec2 normal = glm::normalize(glm::vec2(other->position.x-particle.position.x, other->position.y-particle.position.y));
                    
                    // apply force
                    float kx = (particle.velocity.x - other->velocity.x);
                    float ky = (particle.velocity.y - other->velocity.y);
                    float p = 2.0 * (normal.x * kx + normal.y * ky) / (particle.radius + other->radius);
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