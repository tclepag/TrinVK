//
// Created by lepag on 3/8/2025.
//

#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include <mutex>
#include <thread>

#include "Window.h"
#include "VulkanContext.h"

namespace Trin::Runtime::Core {
    struct RenderWorker {
        CommandBuffer commandBuffer;
        std::atomic_bool completed = false;
    };
class Engine {
public:
    Engine();
    bool init();
    bool run();
    void mainLoop();
    bool shutdown();
private:
    // ==============
    //     VULKAN
    // ==============

    std::shared_ptr<VulkanContext> m_context;

    // ==============
    //      MAIN
    // ==============

    bool m_running = true;

    // ==============
    //     WINDOW
    // ==============

    std::shared_ptr<Window> m_window;
};

}

#endif //ENGINE_H
