//
// Created by lepag on 3/11/25.
//

#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <vulkan/vulkan.hpp>

#include "Core/VulkanContext.h"
#include "Graphics/VulkanProgram.h"
#include "RenderTargets/VulkanRenderTarget.h"

namespace Trin::Runtime::Rendering {
    using namespace vk;
    using namespace Graphics;
    using namespace Core;
    using namespace RenderTargets;

    using Semaphores = std::vector<Semaphore>;
    using Fences = std::vector<Fence>;
    using Images = std::vector<Image>;
    using ImageViews = std::vector<ImageView>;

    using VulkanShaderPrograms = std::vector<VulkanProgram*>;

    class VulkanRenderer {
    public:
        VulkanRenderer();

        ~VulkanRenderer();

        void init();

        // ==============
        //    PROGRAM
        // ==============

        VulkanProgram* createProgram(const char* name, std::string vertexShaderPath, std::string fragmentShaderPath);
        void setProgram(VulkanProgram* program);
        void draw();

        // ==============
        // RENDER TARGETS
        // ==============

        /// Creates a Render Target with class
        template<class T>
        T* createRenderTarget(const char* name);

    private:
        // ==============
        //     VULKAN
        // ==============

        std::shared_ptr<VulkanContext> m_context;

        // ==============
        //      CORE
        // ==============

        uint32_t m_currentFrame = 0;
        const int MAX_FRAMES_IN_FLIGHT = 2;

        // ==============
        //    RENDERING
        // ==============

        std::unordered_map<const char*, VulkanRenderTarget*> m_renderTargets;
        std::unordered_map<const char*, CommandPool> m_commandPools;
        PipelineCache m_pipelineCache;

        CommandPool m_mainCommandPool;
        CommandPool m_transferCommandPool;

        // ===============
        // SYNCHRONIZATION
        // ===============

        Semaphores m_imageAvailableSemaphores;
        Semaphores m_renderFinishedSemaphores;
        Fences m_frameFences;

    };
}

#endif //VULKANRENDERER_H
