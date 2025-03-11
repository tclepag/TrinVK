//
// Created by lepag on 3/11/25.
//

#ifndef VULKANRENDERTARGET_H
#define VULKANRENDERTARGET_H

#include <vulkan/vulkan.hpp>

#include "Core/VulkanContext.h"
#include "Graphics/VulkanProgram.h"

namespace Trin::Runtime::Rendering::RenderTargets {
    using namespace vk;
    using namespace Core;
    using namespace Graphics;

    struct AttachmentConfig {
        Format format;                      // Pixel format
        SampleCountFlagBits samples;        // Multisampling configuration
        ImageUsageFlags usage;              // How the image will be used
        AttachmentLoadOp loadOp;            // What to do with the attachment at start
        AttachmentStoreOp storeOp;          // What to do with the attachment at end
        ImageLayout initialLayout;          // Layout at render pass start
        ImageLayout finalLayout;            // Layout at render pass end
        bool blendEnable;                   // Whether blending is enabled
    };

    class VulkanRenderTarget {
    public:
        VulkanRenderTarget(const std::shared_ptr<VulkanContext> &context);
        virtual ~VulkanRenderTarget();

        virtual void init();
        void cleanup() const;

        // ==============
        //    SETTERS
        // ==============

        void resize(uint32_t width, uint32_t height);

        // ==============
        //    GETTERS
        // ==============

        Image getRenderAsImage() const;
        Extent2D getExtent() const;
        RenderPass getRenderPass() const;
        Image getImage() const;
        DeviceMemory getMemory() const;
        ImageView getImageView() const;
        Framebuffer getFramebuffer() const;
        std::array<uint32_t, 2> getSize() const;

    protected:
        // ==============
        //     CORE
        // ==============

        const std::shared_ptr<VulkanContext> m_context;

        uint32_t m_width = 400;
        uint32_t m_height = 200;

        bool m_dirty = false;

        /// Change ImageLayout types
        /// Useful if an ImageLayout is for rendering but we want to copy it and use it as a texture
        void transitionLayout(CommandBuffer* cmd, ImageLayout oldLayout, ImageLayout newLayout);

        // ==============
        //     RENDER
        // ==============

        Extent2D m_extent;
        RenderPass m_renderPass;
        ClearColorValue m_clearColor = std::array{0.0f, 0.0f, 0.0f, 1.0f};
        ClearValue m_depthClearValue = {{1.0f, 0}};

        AttachmentConfig m_colorAttachment;
        AttachmentConfig m_depthAttachment;

        Image m_image;
        DeviceMemory m_imageMemory; // Store frames as memory
        ImageView m_imageView;
        Framebuffer m_framebuffer;

        std::vector<VulkanProgram*> m_programs;
        std::vector<Pipeline> m_pipelines;

        // ==============
        //    VIRTUALS
        // ==============

        virtual void prepare(CommandBuffer* cmd);
        virtual void finalize(CommandBuffer* cmd);
        virtual void update();

        // ==============
        //     SETUP
        // ==============

        AttachmentConfig createAttachment(Format format, ImageUsageFlags usage);
        void createImageView(ImageAspectFlags aspectFlags, ImageView& imageView);
        void createFramebuffer(Framebuffer& framebuffer);
        void createRenderPass(RenderPass& renderPass);
        void createPipeline(VulkanProgram* program, Pipeline& pipeline);
        void createDescriptorSetLayout(DescriptorSetLayout& descriptorLayout);
        void createDescriptorPool(DescriptorPool& descriptorPool);
        void createDescriptorSet(DescriptorSet& descriptorSet);
    };

}

#endif //VULKANRENDERTARGET_H
