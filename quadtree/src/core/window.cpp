//
// Created by gilmar on 22/04/2022.
//

#include "window.hpp"

#include <cassert>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Window::Window(WindowData data) : data(data) {
    uint glfwInitialized = glfwInit();
    assert(glfwInitialized);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);


    native_window = glfwCreateWindow(data.width, data.height, data.title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(native_window, &this->data);

    glfwMakeContextCurrent(native_window);
    glfwSwapInterval(0);

    uint gladInitialized = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    assert(gladInitialized);

    glViewport(0, 0, data.width, data.height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_MULTISAMPLE);

    std::cout << glfwGetVersionString() << std::endl;

    glfwSetWindowSizeCallback(native_window, [](GLFWwindow* window, int width, int height) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.width = width;
        data.height = height;

        float half_width = (float) width / 2.f;
        float half_height = (float) height / 2.f;

        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::ortho(-half_width, half_width, -half_height, half_height,-1000.0f, 1000.0f);

//        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        glViewport(0, 0, width, height);
    });
}

Window::~Window() {
    glfwDestroyWindow(native_window);
    glfwTerminate();
}
