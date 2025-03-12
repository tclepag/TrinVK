//
// Created by lepag on 3/12/25.
//

#include "VulkanContext.h"

#include <GLFW/glfw3.h>

namespace Trin::Runtime::Core {
    void VulkanContext::init() {
        createInstance();
        createDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
    }

    void VulkanContext::createInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Trin";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Trin";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

        std::vector<const char*> extensions = getRequiredExtensions();
        std::vector layers = {
            "VK_LAYER_KHRONOS_validation"
        };

        // Create instance
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(std::size(extensions));
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(std::size(layers));
        createInfo.ppEnabledLayerNames = layers.data();

        m_extensions = extensions;
        m_layers = layers;
    }

    void VulkanContext::createDebugMessenger() {
    }

    void VulkanContext::createSurface() {
    }

    void VulkanContext::pickPhysicalDevice() {
    }

    void VulkanContext::createLogicalDevice() {
    }

    std::vector<const char *> VulkanContext::getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        return extensions;
    }
}
