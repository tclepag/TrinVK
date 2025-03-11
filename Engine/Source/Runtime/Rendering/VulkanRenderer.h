//
// Created by lepag on 3/11/25.
//

#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <vulkan/vulkan.hpp>

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
    using FrameBuffers = std::vector<Framebuffer>;

    class VulkanRenderer {
    public:
        VulkanRenderer();

        ~VulkanRenderer();

        void init();

        // ==============
        //     STATES
        // ==============

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
        //   SWAP CHAIN
        // ==============

        SwapchainKHR m_swapChain;
        Images m_swapChainImages;
        ImageViews m_swapChainImageViews;
        Format m_swapChainFormat;
        Extent2D m_swapChainExtent;

        // ==============
        //    RENDERING
        // ==============

        RenderPass m_renderPass;
        FrameBuffers m_frameBuffers;
        PipelineLayout m_pipelineLayout;
        Pipeline m_pipeline;

        // ===============
        // SYNCHRONIZATION
        // ===============

        Semaphores m_imageAvailableSemaphores;
        Semaphores m_renderFinishedSemaphores;
        Fences m_frameFences;

    };
}

#endif //VULKANRENDERER_H
