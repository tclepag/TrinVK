//
// Created by lepag on 3/9/25.
//

#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <iostream>

#include "Helpers/Types.h"
#include "Math/Vector2.h"



namespace Trin::Runtime::Core {
    using namespace Trin::Math;
    struct WindowCreateInfo {
        String title;
        String icoPath;
        Vector2 size;
    };
class Window {
public:
    explicit Window(const WindowCreateInfo& info);
    ~Window();

    [[nodiscard]] SDL_Window* GetWindow() const {
        return m_window;
    }

    [[nodiscard]] Vector2* GetSize() const {
        int width = 0, height = 0;
        SDL_GetWindowSize(m_window, &width, &height);
        return new Vector2(static_cast<float>(width), static_cast<float>(height));
    }

    void cleanup() const {
        delete this;
    }
private:
    SDL_Window* m_window;
};

}

#endif //WINDOW_H
