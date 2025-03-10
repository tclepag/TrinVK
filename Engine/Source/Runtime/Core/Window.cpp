//
// Created by lepag on 3/9/25.
//

#include "Window.h"

namespace Trin::Runtime::Core {
    Window::Window(const WindowCreateInfo& info) {
        m_window = SDL_CreateWindow(
            info.title.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOW_VULKAN
        );

        if (!m_window) {
            std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
            throw std::runtime_error(SDL_GetError());
        }
    }

    Window::~Window() {
        SDL_DestroyWindow(m_window);
    }
}
