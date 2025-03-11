//
// Created by lepag on 3/11/25.
//

#include "VulkanRenderer.h"

namespace Trin::Runtime::Rendering {
    VulkanRenderer::VulkanRenderer() {
        CommandPoolCreateInfo graphicsPoolInfo;
        graphicsPoolInfo.setQueueFamilyIndex(m_context->getGraphicsQueue());
        graphicsPoolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        m_mainCommandPool = m_context->getDevice().createCommandPool(graphicsPoolInfo);

        CommandPoolCreateInfo transferPoolInfo;
        transferPoolInfo.setQueueFamilyIndex(transferQueueIndex);
        transferPoolInfo.setFlags(vk::CommandPoolCreateFlagBits::eTransient);
        m_transferCommandPool = m_context->getDevice().createCommandPool(transferPoolInfo);
    }

    VulkanRenderer::~VulkanRenderer() {
    }

    void VulkanRenderer::init() {

    }

    VulkanProgram * VulkanRenderer::createProgram(const char *name, std::string vertexShaderPath,
        std::string fragmentShaderPath) {
    }

    void VulkanRenderer::setProgram(VulkanProgram *program) {
    }

    void VulkanRenderer::draw() {
    }

    template<class T>
    T * VulkanRenderer::createRenderTarget(const char *name) {
    }

    void VulkanRenderer::createSwapChain() {
    }

    void VulkanRenderer::createImageView() {
    }

    void VulkanRenderer::createRenderPass() {
    }

    void VulkanRenderer::createFrameBuffer() {
    }
}
