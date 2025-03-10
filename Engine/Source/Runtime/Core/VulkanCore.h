//
// Created by lepag on 3/9/25.
//

#ifndef VULKANCORE_H
#define VULKANCORE_H

#include <iostream>
#include <set>
#include <vulkan/vulkan.hpp>

#include "Window.h"
#include "Helpers/Types.h"
#include "Helpers/System.h"

namespace Trin::Runtime::Core {
    using namespace Helpers;
    using namespace vk;
    struct VulkanInitializationInfo {
        Window* window;
        String appName;
        String engineName;
        std::tuple<uint32_t, uint32_t, uint32_t> appVersion;
        std::vector<const char*> layerNames = {};
        std::vector<const char*> extensionNames = {};
        bool enableValidationLayers;
    };
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool isComplete() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };
    struct PhysicalDeviceDetails {
        PhysicalDevice device;
        std::string name;
        PhysicalDeviceType type;
        uint32_t score;
    };
class VulkanCore {
public:
    // Constructors/Destructors
    VulkanCore();
    ~VulkanCore();

    // Prevent copying
    VulkanCore(const VulkanCore&) = delete;
    VulkanCore& operator=(const VulkanCore&) = delete;

    // Moving operations
    VulkanCore(VulkanCore&&) noexcept = default;
    VulkanCore& operator=(VulkanCore&&) noexcept = default;


    /// Boots Vulkan and creates instances necessary
    bool initialize(VulkanInitializationInfo& info);
    /// Cleans up Vulkan instances
    bool cleanup() const;

    /// Returns the Graphics Queue
    [[nodiscard]] Queue getGraphicsQueue() const {
        return m_graphicsQueue;
    }

    [[nodiscard]] Queue getPresentQueue() const {
        return m_presentQueue;
    }

    /// Returns whether Vulkan supports the extension
    bool isExtensionSupported(const String& extension);
private:
    // Vulkan Core Objects
    /// Core layers
    std::vector<const char*> layerNames = {
        "VK_LAYER_KHRONOS_validation"
    };
    /// Core extensions
    std::vector<const char*> extensionNames = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    /// The main Vulkan Instance
    Instance m_instance;
    /// The computer's graphics card
    PhysicalDevice m_physicalDevice;
    /// The connection from the application to the GPU
    Device m_device;
    /// The connection from Vulkan to the application's window
    SurfaceKHR m_surface;

    /// Logical Device Graphics Queue
    Queue m_graphicsQueue;
    /// Logical Device Present Queue
    Queue m_presentQueue;

    /// Checks if our extension list supports the use of this physical device
    bool isDeviceSupported(const PhysicalDevice& physicalDevice);
    /// Find the queue families for this physicalDevice
    QueueFamilyIndices findQueueFamilies(const PhysicalDevice& physicalDevice);
    /// Retrieves the most powerful physical device on this computer
    PhysicalDevice getBestDevice();

    // Vulkan Debugging
    /// Captures and handles errors/debug messages
    DebugUtilsMessengerEXT m_debugMessenger;

    /// The internal debug callback
    static VKAPI_ATTR Bool32 VKAPI_CALL debugCallback(
    DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    DebugUtilsMessageTypeFlagsEXT messageType,
    const DebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);
};

}

#endif //VULKANCORE_H
