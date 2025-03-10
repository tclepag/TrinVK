//
// Created by lepag on 3/9/2025.
//

#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include "Core/VulkanCore.h"
#include "Math/Vector4.h"
#include <fstream>

namespace Trin::Runtime::Rendering {
    using namespace Math;
    using namespace Core;

    class VulkanRenderer {
    public:
        VulkanRenderer(VulkanCore *core);

        ~VulkanRenderer();

        void init();

        void cleanup();

        void render();

        void recreateSwapchain();

        void setClearColor(Vector4 color) {
            clearColor.set(color);
        }

    private:
        // Core Variables
        VulkanCore *m_core;
        Device m_device{};

        // Renderer Variables
        Vector4 clearColor = Vector4::zero();
        float depthClear = 1.0f;
        uint32_t stencilClear = 0;

        // Swapchain
        SwapchainKHR m_swapChain;
        std::vector<Image> m_swapChainImages;
        Format m_swapChainImageFormat = Format::eUndefined;
        VkExtent2D m_swapChainExtent{};
        std::vector<VkImageView> m_swapChainImageViews;
        std::vector<VkFramebuffer> m_swapChainFramebuffers;

        void clearSwapchains();

        // Pipeline
        RenderPass m_renderPass;
        PipelineLayout m_pipelineLayout;
        Pipeline m_pipeline;

        // Command
        CommandPool m_commandPool;
        std::vector<CommandBuffer> m_commandBuffers;

        // Synchronization
        std::vector<Semaphore> m_imageAvailableSemaphores;
        std::vector<Semaphore> m_renderFinishedSemaphores;
        std::vector<Fence> m_inFlightFences;
        size_t m_currentFrame = 0;
        const int MAX_FRAMES_IN_FLIGHT = 2;

        SurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<SurfaceFormatKHR> &availableFormats) {
            // Preferred format is SRGB with B8G8R8A8 (widely supported)
            for (const auto &availableFormat: availableFormats) {
                if (availableFormat.format == Format::eB8G8R8A8Srgb &&
                    availableFormat.colorSpace == ColorSpaceKHR::eSrgbNonlinear) {
                    return availableFormat;
                }
            }

            // If preferred format isn't available, just use the first format
            return availableFormats[0];
        }

        PresentModeKHR chooseSwapPresentMode(
            const std::vector<PresentModeKHR> &availablePresentModes) {
            // Look for mailbox mode (triple buffering) which gives smooth visuals with low latency
            for (const auto &availablePresentMode: availablePresentModes) {
                if (availablePresentMode == PresentModeKHR::eMailbox) {
                    return availablePresentMode;
                }
            }

            // FIFO (V-Sync) is guaranteed to be available
            return PresentModeKHR::eFifo;
        }

        [[nodiscard]] Extent2D chooseSwapExtent(
            const SurfaceCapabilitiesKHR &capabilities) const {
            // If the current extent is set to max value, the window manager allows choosing an extent different from window resolution
            if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
                return capabilities.currentExtent;
            }
            const auto windowSize = m_core->getWindow()->GetSize();
            const int width = static_cast<int>(windowSize.x);
            const int height = static_cast<int>(windowSize.y);

            Extent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            // Clamp between the min and max extents supported by the device
            actualExtent.width = std::clamp(actualExtent.width,
                                            capabilities.minImageExtent.width,
                                            capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height,
                                             capabilities.minImageExtent.height,
                                             capabilities.maxImageExtent.height);

            return actualExtent;
        }

        ShaderModule createShaderModule(const std::vector<char>& code) {
            // Create shader module create info
            ShaderModuleCreateInfo createInfo;
            createInfo.codeSize = code.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    
            try {
                return m_device.createShaderModule(createInfo);
            } catch (const SystemError& e) {
                throw std::runtime_error("Failed to create shader module: " + std::string(e.what()));
            }
        }
    };
}

#endif //VULKANRENDERER_H
