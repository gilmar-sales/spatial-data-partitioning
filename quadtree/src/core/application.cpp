//
// Created by gilmar on 22/04/2022.
//

#include "application.hpp"
#include "shaders.hpp"

#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


float Application::delta_time = 0.0f;

Application::Application(WindowData data): window(data), systems(0) {
    //Initialize shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    checkShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    checkShader(fragmentShader);

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertexShader);
    glAttachShader(shader_program, fragmentShader);
    glLinkProgram(shader_program);

    glUseProgram(shader_program);

    int success;
    char infoLog[512];
// check for linking errors
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glm::mat4 projection = glm::mat4(1.0f);

    float half_width = (float) data.width / 2.f;
    float half_height = (float) data.height / 2.f;

    projection = glm::ortho(-half_width, half_width, -half_height, half_height,-1000.0f, 1000.0f);

    glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, &projection[0][0]);
}

void Application::run() {
    double frameTimeAccumulator = 0;
    unsigned framesPerSecond = 0;
    while(!glfwWindowShouldClose(Application::get()->get_window().get_native_window()))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();
        for (auto& system : systems) {
            system();
        }
        glfwSwapBuffers(window.get_native_window());

        static double last_time = glfwGetTime();
        double now = glfwGetTime();
        delta_time = (float) (now - last_time);
        last_time = now;

        frameTimeAccumulator += delta_time;
        framesPerSecond++;
        if (frameTimeAccumulator >= 1)
        {
            std::stringstream fmt;

            fmt <<  window.get_title() << " - FPS: " << framesPerSecond << " - Particles count: " << particles_count;

            glfwSetWindowTitle(window.get_native_window(), fmt.str().c_str());
            frameTimeAccumulator = 0;
            framesPerSecond = 0;
        }
    }
}

