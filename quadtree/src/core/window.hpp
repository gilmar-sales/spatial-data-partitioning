#ifndef SPATIAL_DATA_PARTITIONING_WINDOW_HPP
#define SPATIAL_DATA_PARTITIONING_WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

struct WindowData {
    std::string title;
    int width;
    int height;
};

class Window {
public:

    GLFWwindow* get_native_window() { return native_window; }
    std::string_view get_title() const { return data.title; }
    int get_width() const { return data.width; }
    int get_height() const { return data.height; }

    explicit Window(WindowData data);
    ~Window();
private:
    GLFWwindow* native_window;
    WindowData data;
};


#endif //SPATIAL_DATA_PARTITIONING_WINDOW_HPP
