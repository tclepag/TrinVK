//
// Created by lepag on 3/8/2025.
//

#include "Engine.h"

namespace Trin::Runtime::Core {
    Engine::Engine() {}

    bool Engine::init() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }
        WindowCreateInfo createWindowInfo{};
        createWindowInfo.size = Vector2(800, 600);
        createWindowInfo.title = "TrinVK Engine";

        m_window = std::make_unique<Window>(createWindowInfo);

        VulkanCreateInfo createInfo{};
        createInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
        createInfo.applicationName = "TrinVK Engine";
        createInfo.enableValidationLayers = true;
        createInfo.window = m_window;

        m_context = std::make_shared<VulkanContext>();
        m_context->init(createInfo);

        return true;
    }

    bool Engine::run() {
        m_running = true;
        mainLoop();
        return true;
    }

    void Engine::mainLoop() {
        while (m_running) {
            if (glfwWindowShouldClose(m_window->GetWindow())) {
                m_running = false;
            }
            glfwPollEvents();

            if (glfwGetKey(m_window->GetWindow(), GLFW_KEY_ESCAPE)) {
                glfwSetWindowShouldClose(m_window->GetWindow(), true);
            }

            // Render here..

            glfwSwapBuffers(m_window->GetWindow());
        }
        std::cout << "done" << std::endl;
    }

    bool Engine::shutdown() {
        return true;
    }
}
