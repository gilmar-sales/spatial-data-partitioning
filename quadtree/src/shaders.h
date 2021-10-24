#pragma once


#define checkShader(shader)                                 \
{                                                           \
    GLint status;                                           \
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);      \
                                                            \
    if(status == -1) {                                      \
        char buffer[512];                                   \
        glGetShaderInfoLog(shader, 512, NULL, buffer);      \
                                                            \
        std::cout << buffer << std::endl;                   \
    } else {                                                \
        std::cout << #shader << " compiled successfuly!\n"; \
    }                                                       \
                                                            \
}

const char* vertexSource = R"glsl(
    #version 330 core
    layout(location = 0) in vec2 position;

    uniform mat4 projection;
    uniform mat4 model;

    void main()
    {
        gl_Position = projection * model * vec4(position, 1.0, 1.0);
    }
)glsl";


const char* fragmentSource = R"glsl(
    #version 330 core
    uniform vec3 color;

    out vec4 outColor;

    void main()
    {
        outColor = vec4(color, 1.0);
    }
)glsl";