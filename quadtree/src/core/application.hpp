//
// Created by gilmar on 22/04/2022.
//

#ifndef SPATIAL_DATA_PARTITIONING_APPLICATION_HPP
#define SPATIAL_DATA_PARTITIONING_APPLICATION_HPP

#include "window.hpp"

#include <functional>

class Application {
public:
    void run();
    Window& get_window() { return window; }
    void register_system(std::function<void()> system) { systems.push_back(system); }
    unsigned get_shader_program() { return shader_program; }

    static Application* get() {
        static Application* instance = create_application();
        return instance;
    };

    static float delta_time;

protected:
    static Application* create_application();
    explicit Application(WindowData data);
    ~Application() = default;
private:
    Window window;
    std::vector<std::function<void()>> systems;
    unsigned shader_program;
};


#endif //SPATIAL_DATA_PARTITIONING_APPLICATION_HPP
