//
// Created by lepag on 3/11/25.
//

#include "VulkanRenderTarget.h"

namespace Trin::Runtime::Rendering::RenderTargets {
    VulkanRenderTarget::VulkanRenderTarget(const std::shared_ptr<VulkanContext> &context): m_context(context),
        m_colorAttachment({}),
        m_depthAttachment({}) {
    }

    VulkanRenderTarget::~VulkanRenderTarget() = default;

    void VulkanRenderTarget::init() {
    }

    void VulkanRenderTarget::transitionLayout(CommandBuffer *cmd, ImageLayout oldLayout, ImageLayout newLayout) {
    }

    void VulkanRenderTarget::prepare(CommandBuffer *cmd) {
    }

    void VulkanRenderTarget::finalize(CommandBuffer *cmd) {
    }

    void VulkanRenderTarget::update() {
    }

    AttachmentConfig VulkanRenderTarget::createAttachment(Format format, ImageUsageFlags usage) {
    }

    void VulkanRenderTarget::createImageView() {
    }

    void VulkanRenderTarget::createFramebuffer() {
    }

    void VulkanRenderTarget::createRenderPass() {
    }

    void VulkanRenderTarget::createDescriptorSetLayout() {
    }

    void VulkanRenderTarget::createDescriptorPool() {
    }

    void VulkanRenderTarget::createDescriptorSet() {
    }

    void VulkanRenderTarget::cleanup() const {
        delete this;
    }

    void VulkanRenderTarget::resize(const uint32_t width, const uint32_t height) {
        m_width = width;
        m_height = height;
        m_dirty = true;
    }

    Image VulkanRenderTarget::getRenderAsImage() const {

    }

    Extent2D VulkanRenderTarget::getExtent() const {
        return m_extent;
    }

    RenderPass VulkanRenderTarget::getRenderPass() const {
        return m_renderPass;
    }

    Image VulkanRenderTarget::getImage() const {
        return m_image;
    }

    DeviceMemory VulkanRenderTarget::getMemory() const {
        return m_imageMemory;
    }

    ImageView VulkanRenderTarget::getImageView() const {
        return m_imageView;
    }

    Framebuffer VulkanRenderTarget::getFramebuffer() const {
        return m_framebuffer;
    }

    std::array<uint32_t, 2> VulkanRenderTarget::getSize() const {
        return {m_width, m_height};
    }
}
