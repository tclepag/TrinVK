//
// Created by lepag on 3/9/25.
//

#include "Window.h"

namespace Trin::Runtime::Core {
    Window::Window(const WindowCreateInfo& info): m_context(info.context), m_surface(nullptr) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_window = glfwCreateWindow(
            info.size.x, info.size.y,
            info.title.c_str(),
            nullptr,
            nullptr
        );

        if (!m_window) {
            throw std::runtime_error("Failed to create GLFW window");
        }
    }

    Window::~Window() {
        glfwDestroyWindow(m_window);
    }
}
