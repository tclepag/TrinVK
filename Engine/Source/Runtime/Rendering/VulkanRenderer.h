//
// Created by lepag on 3/11/25.
//

#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <vulkan/vulkan.hpp>

#include "VulkanRenderTarget.h"
#include "Core/VulkanContext.h"
#include "Graphics/VulkanProgram.h"

namespace Trin::Runtime::Rendering {
    using namespace vk;
    using namespace Graphics;
    using namespace Core;

    using Semaphores = std::vector<Semaphore>;
    using Fences = std::vector<Fence>;
    using Images = std::vector<Image>;
    using ImageViews = std::vector<ImageView>;
    using RenderTargets = std::vector<VulkanRenderTarget>;

    using VulkanShaderPrograms = std::vector<VulkanProgram*>;

    class VulkanRenderer {
    public:
        VulkanRenderer();

        ~VulkanRenderer();

        void init();

        // ==============
        //     STATES
        // ==============

        VulkanProgram* newProgram(const char* name, std::string vertexShaderPath, std::string fragmentShaderPath);
        void setProgram(VulkanProgram* program);
        void draw();


    private:
        // ==============
        //     VULKAN
        // ==============

        std::shared_ptr<VulkanContext> m_context;

        // ==============
        //      CORE
        // ==============

        VulkanProgram* m_currentProgram;

        uint32_t m_currentFrame = 0;
        const int MAX_FRAMES_IN_FLIGHT = 2;

        // ==============
        //     SETUP
        // ==============

        void createSwapChain();
        void createImageView();
        void createRenderPass();
        void createFrameBuffer();

        // ==============
        //    RENDERING
        // ==============

        VulkanShaderPrograms m_shaderPrograms;

        // ===============
        // SYNCHRONIZATION
        // ===============

        Semaphores m_imageAvailableSemaphores;
        Semaphores m_renderFinishedSemaphores;
        Fences m_frameFences;

    };
}

#endif //VULKANRENDERER_H
