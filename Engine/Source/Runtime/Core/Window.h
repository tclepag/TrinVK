//
// Created by lepag on 3/9/25.
//

#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <iostream>

#include "VulkanContext.h"
#include "Helpers/Types.h"
#include "Math/Vector2.h"


namespace Trin::Runtime::Core {
    using namespace Trin::Math;
    struct WindowCreateInfo {
        String title;
        String icoPath;
        Vector2 size;
        std::shared_ptr<VulkanContext> context;
    };
class Window {
public:
    explicit Window(const WindowCreateInfo& info);
    ~Window();

    [[nodiscard]] GLFWwindow* GetWindow() const {
        return m_window;
    }

    [[nodiscard]] std::array<int, 2> GetSize() const {
        int width = 0, height = 0;
        glfwGetWindowSize(m_window, &width, &height);
        return {width, height};
    }

    void cleanup() const {
        delete this;
    }
private:
    // ==============
    //     VULKAN
    // ==============

    std::shared_ptr<VulkanContext> m_context;
    VkSurfaceKHR m_surface;

    // ==============
    //     WINDOW
    // ==============

    GLFWwindow* m_window;
};

}

#endif //WINDOW_H
