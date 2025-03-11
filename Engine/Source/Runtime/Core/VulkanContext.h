//
// Created by lepag on 3/10/2025.
//

#ifndef VULKANCONTEXT_H
#define VULKANCONTEXT_H

#include <iostream>
#include <optional>
#include <vulkan/vulkan.hpp>
#include "Math/Vector3.h"

namespace Trin::Constants {
    constexpr auto ENGINE_NAME = "TrinVK";
    constexpr auto ENGINE_VERSION = VK_MAKE_API_VERSION(0, 1, 0, 0);
}

namespace Trin::Runtime::Core {
    using namespace Math;
    using namespace vk;

    class Window;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool isComplete() const {
            return graphicsFamily.has_value() &&
                   presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        SurfaceCapabilitiesKHR capabilities;
        std::vector<SurfaceFormatKHR> formats;
        std::vector<PresentModeKHR> presentModes;

        [[nodiscard]] bool isAdequate() const {
            return !formats.empty() && !presentModes.empty();
        }
    };

    struct RankedPhysicalDevice {
        PhysicalDevice physicalDevice;
        uint32_t score = 0;
    };

    struct VulkanCreateInfo {
        const char *applicationName = "TrinVK";
        uint32_t applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        bool enableValidationLayers = false;
        std::shared_ptr<Window> window;
    };

    class VulkanContext {
    public:
        explicit VulkanContext();

        ~VulkanContext();

        // ==============
        //      MAIN
        // ==============


        /// Starts initialization of Vulkan
        void init(const VulkanCreateInfo &info);

        /// Cleans up Vulkan
        void cleanup();

        // ==============
        //    GETTERS
        // ==============

        [[nodiscard]] Instance getInstance() const { return m_instance; }
        [[nodiscard]] Device getDevice() const { return m_device; }
        [[nodiscard]] PhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
        [[nodiscard]] Queue getGraphicsQueue() const { return m_graphicsQueue; }
        [[nodiscard]] Queue getPresentQueue() const { return m_presentQueue; }

    private:
        // ==============
        //  VULKAN CORE
        // ==============

        Instance m_instance;
        Device m_device;
        SurfaceKHR m_surface;
        std::shared_ptr<Window> m_window;
        PhysicalDevice m_physicalDevice;
        Queue m_graphicsQueue;
        Queue m_presentQueue;
        uint32_t m_apiVersion;
        const char *m_applicationName;
        bool m_enableValidationLayers;

        std::vector<const char *> m_extensionNames;
        std::vector<const char *> m_layerNames;

        /// Creates the Vulkan instance
        void initInstance();

        /// Creates the Vulkan logical device
        void initDevice();

        /// Creates the Vulkan Surface
        void initSurface();

        /// Gets the Vulkan physical device
        void initPhysicalDevice();

        /// Creates the queues
        void initQueues();

        /// Gets all Queue Families associated with this physicalDevice
        QueueFamilyIndices findQueueFamilies(const PhysicalDevice &physicalDevice);

        /// Checks if a physicalDevice is suitable for Vulkan
        bool isDeviceSuitable(const PhysicalDevice &physicalDevice);
        /// Returns a rating of GPU effectiveness
        uint32_t rateDevice(const PhysicalDevice &physicalDevice);

        // ==============
        //  VULKAN DEBUG
        // ==============

        DebugUtilsMessengerEXT m_debugger;

        /// Creates a new debugMessenger
        void createDebugUtilsMessengerEXT();

        /// Destroys the current debug messenger
        void destroyDebugUtilsMessengerEXT() const;

        /// Checks if Vulkan validation layers are supported
        bool isValidationLayersSupported();

        /// Returns all extensions that are required based on validation layers
        std::vector<const char *> getRequiredExtensions() const;

        /// Vulkan Debugger Helper
        VKAPI_ATTR static VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData) {

            // Print the validation layer message
            std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

            // Return VK_FALSE to indicate we want Vulkan to continue execution
            return VK_FALSE;
        }

        // ==============
        //     HELPERS
        // ==============

        /// Checks if all extensions required for this physicalDevice are being supported
        bool checkDeviceExtensionSupport(const PhysicalDevice &physicalDevice);

        /// Gets the swapChain support details for this physicalDevice
        SwapChainSupportDetails getSwapChainSupport(const PhysicalDevice &physicalDevice) const;
    };
}

#endif //VULKANCONTEXT_H
