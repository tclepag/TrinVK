//
// Created by lepag on 3/9/2025.
//

#include "VulkanRenderer.h"


std::vector<char> readFile(const std::string& filename) {
    // Open the file with cursor at the end to determine size
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Get file size and allocate buffer
    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    // Go back to beginning of file and read all data
    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

    file.close();
    return buffer;
}

namespace Trin::Runtime::Rendering {
    VulkanRenderer::VulkanRenderer(VulkanCore *core) {
        this->m_core = core;
        this->m_device = core->getDevice();
        init();
    }

    VulkanRenderer::~VulkanRenderer() {
    }

    void VulkanRenderer::init() {
        // Setup swapchain
        auto swapChainSupport = m_core->querySwapChainSupport(m_core->getPhysicalDevice());

        // Get swapSurface format, present mode and extent
        auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        auto presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        auto extent = chooseSwapExtent(swapChainSupport.capabilities);

        // Get the amount of images in swapchain
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.
            maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        // Setup swap chain info
        SwapchainCreateInfoKHR createInfo{};
        createInfo.surface = m_core->getSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = ImageUsageFlagBits::eColorAttachment;

        // Setup Queue family handling
        auto indices = m_core->findQueueFamilies(m_core->getPhysicalDevice());
        std::array queueFamilyIndices = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
            createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
        } else {
            createInfo.imageSharingMode = SharingMode::eExclusive;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = nullptr;

        // Create the swap chain
        try {
            m_swapChain = m_device.createSwapchainKHR(createInfo);
        } catch (const SystemError &e) {
            throw std::runtime_error("Failed to create swap chain: " + std::string(e.what()));
        }

        // Retrieve swap chain images
        m_swapChainImages = m_device.getSwapchainImagesKHR(m_swapChain);

        // Store format and extent for later use
        m_swapChainImageFormat = surfaceFormat.format;
        m_swapChainExtent = extent;

        // Setup imageViews
        m_swapChainImageViews.resize(m_swapChainImages.size());

        for (uint32_t i = 0; i < m_swapChainImages.size(); i++) {
            ImageViewCreateInfo imageCreateInfo{};
            imageCreateInfo.image = m_swapChainImages[i];
            imageCreateInfo.viewType = ImageViewType::e2D;
            imageCreateInfo.format = m_swapChainImageFormat;

            // Default mapping for color channels
            imageCreateInfo.components.r = ComponentSwizzle::eIdentity;
            imageCreateInfo.components.g = ComponentSwizzle::eIdentity;
            imageCreateInfo.components.b = ComponentSwizzle::eIdentity;
            imageCreateInfo.components.a = ComponentSwizzle::eIdentity;

            // Image is used as a color attachment
            imageCreateInfo.subresourceRange.aspectMask = ImageAspectFlagBits::eColor;
            imageCreateInfo.subresourceRange.baseMipLevel = 0;
            imageCreateInfo.subresourceRange.levelCount = 1;
            imageCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageCreateInfo.subresourceRange.layerCount = 1;

            try {
                m_swapChainImageViews[i] = m_device.createImageView(imageCreateInfo);
            } catch (const SystemError &e) {
                throw std::runtime_error("Failed to create image views: " + std::string(e.what()));
            }
        }

        // Build render pass

        AttachmentDescription colorAttachment;
        colorAttachment.format = m_swapChainImageFormat;
        colorAttachment.samples = SampleCountFlagBits::e1;
        colorAttachment.loadOp = AttachmentLoadOp::eClear;
        colorAttachment.storeOp = AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = ImageLayout::eUndefined;
        colorAttachment.finalLayout = ImageLayout::ePresentSrcKHR;

        // Setup color attachment reference
        AttachmentReference colorAttachmentRef;
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = ImageLayout::eColorAttachmentOptimal;

        // Setup subpass
        SubpassDescription subpass;
        subpass.pipelineBindPoint = PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        // Setup subpass dependency for image layout transitions
        SubpassDependency dependency;
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.srcAccessMask = AccessFlagBits::eNone;
        dependency.dstStageMask = PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.dstAccessMask = AccessFlagBits::eColorAttachmentWrite;

        // Create the render pass
        RenderPassCreateInfo renderPassInfo;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        try {
            m_renderPass = m_device.createRenderPass(renderPassInfo);
        } catch (const SystemError &e) {
            throw std::runtime_error("Failed to create render pass: " + std::string(e.what()));
        }

        // Graphics pipeline
        auto vertShaderCode = readFile("shaders/vert.spv");
    auto fragShaderCode = readFile("shaders/frag.spv");
    
    // Create shader modules
    ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    ShaderModule fragShaderModule = createShaderModule(fragShaderCode);
    
    // Setup vertex shader stage
    PipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.stage = ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    
    // Setup fragment shader stage
    PipelineShaderStageCreateInfo fragShaderStageInfo;
    fragShaderStageInfo.stage = ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";
    
    std::array<PipelineShaderStageCreateInfo, 2> shaderStages = {vertShaderStageInfo, fragShaderStageInfo};
    
    // Setup vertex input
    PipelineVertexInputStateCreateInfo vertexInputInfo;
    // For now, we're not providing any vertex data
    
    // Setup input assembly
    PipelineInputAssemblyStateCreateInfo inputAssembly;
    inputAssembly.topology = PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    // Setup viewport and scissor
    Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swapChainExtent.width);
    viewport.height = static_cast<float>(m_swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    Rect2D scissor;
    scissor.offset = Offset2D{0, 0};
    scissor.extent = m_swapChainExtent;
    
    PipelineViewportStateCreateInfo viewportState;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    
    // Setup rasterizer
    PipelineRasterizationStateCreateInfo rasterizer;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = CullModeFlagBits::eBack;
    rasterizer.frontFace = FrontFace::eClockwise;
    rasterizer.depthBiasEnable = VK_FALSE;
    
    // Setup multisampling
    PipelineMultisampleStateCreateInfo multisampling;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = SampleCountFlagBits::e1;
    
    // Setup color blending
    PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = ColorComponentFlagBits::eR | ColorComponentFlagBits::eG | 
                                         ColorComponentFlagBits::eB | ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_FALSE;
    
    PipelineColorBlendStateCreateInfo colorBlending;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    
    // Setup pipeline layout
    PipelineLayoutCreateInfo pipelineLayoutInfo;
    // For now, no descriptor sets or push constants
    
    try {
        m_pipelineLayout = m_device.createPipelineLayout(pipelineLayoutInfo);
    } catch (const SystemError& e) {
        throw std::runtime_error("Failed to create pipeline layout: " + std::string(e.what()));
    }
    
    // Create graphics pipeline
    GraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = 0;
    
    try {
        auto pipelineResult = m_device.createGraphicsPipeline(nullptr, pipelineInfo);
        m_pipeline = pipelineResult.value;
    } catch (const SystemError& e) {
        throw std::runtime_error("Failed to create graphics pipeline: " + std::string(e.what()));
    }
    
    // Cleanup shader modules
    m_device.destroyShaderModule(fragShaderModule);
    m_device.destroyShaderModule(vertShaderModule);

        // Create Command pool
        auto commandQueueFamilyIndices = m_core->findQueueFamilies(m_core->getPhysicalDevice());

        CommandPoolCreateInfo poolInfo;
        poolInfo.queueFamilyIndex = commandQueueFamilyIndices.graphicsFamily.value();
        poolInfo.flags = CommandPoolCreateFlagBits::eResetCommandBuffer;

        try {
            m_commandPool = m_device.createCommandPool(poolInfo);
        } catch (const SystemError& e) {
            throw std::runtime_error("Failed to create command pool: " + std::string(e.what()));
        }

        // Create Command buffers
        m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        CommandBufferAllocateInfo allocInfo;
        allocInfo.commandPool = m_commandPool;
        allocInfo.level = CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

        try {
            m_commandBuffers = m_device.allocateCommandBuffers(allocInfo);
        } catch (const SystemError& e) {
            throw std::runtime_error("Failed to allocate command buffers: " + std::string(e.what()));
        }

        // Setup Synchronization
        m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        SemaphoreCreateInfo semaphoreInfo{};

        FenceCreateInfo fenceInfo;
        fenceInfo.flags = FenceCreateFlagBits::eSignaled;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            try {
                m_imageAvailableSemaphores[i] = m_device.createSemaphore(semaphoreInfo);
                m_renderFinishedSemaphores[i] = m_device.createSemaphore(semaphoreInfo);
                m_inFlightFences[i] = m_device.createFence(fenceInfo);
            } catch (const SystemError& e) {
                throw std::runtime_error("Failed to create synchronization objects: " + std::string(e.what()));
            }
        }
    }

    void VulkanRenderer::cleanup() {
    }

    void VulkanRenderer::render() {
    }

    void VulkanRenderer::recreateSwapChain() {
        // Handle minimization
        auto windowSize = m_core->getWindow()->GetSize();
        while (windowSize.x == 0 || windowSize.y == 0) {
            windowSize = m_core->getWindow()->GetSize();
            m_core->waitForEvents();
        }
    
        // Wait for device to be idle before recreating resources
        m_device.waitIdle();

        // Cleanup old resources
        clearSwapchains();

        // Recreate swapchain resources
        createSwapChain();
        createImageViews();
        createFramebuffers();
    }

    void VulkanRenderer::clearSwapchains() {
    }
}
