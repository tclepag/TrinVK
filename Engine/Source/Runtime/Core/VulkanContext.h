//
// Created by lepag on 3/12/25.
//

#ifndef VULKANCONTEXT_H
#define VULKANCONTEXT_H

#include <optional>
#include <set>
#include <vulkan/vulkan.hpp>

namespace Trin::Runtime::Core {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool complete() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };
    struct Queue {
        Queue(VkDevice logicalDevice, uint32_t index) {
            m_id = index;
            vkGetDeviceQueue(logicalDevice, index, 0, &m_queue);
        }
        ~Queue() {
            m_id = 0;
            m_queue = VK_NULL_HANDLE;
            m_physicalDevice = VK_NULL_HANDLE;
        }
    private:
        uint32_t m_id = 0;
        VkQueue m_queue = VK_NULL_HANDLE;
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    };
    struct PhysicalDevice {
    private:
        [[nodiscard]] QueueFamilyIndices getQueueFamilies() const {
            QueueFamilyIndices indices;

            // Find queue family count
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
            // Populate vector full of queues
            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

            for (uint32_t i = 0; i < queueFamilyCount; i++) {
                if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    indices.graphicsFamily = i;
                }

                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
                if (presentSupport) {
                    indices.presentFamily = i;
                }
            }
            return indices;
        }
    public:
        VkPhysicalDevice physicalDevice;
        VkPhysicalDeviceProperties physicalDeviceProperties{};
        VkPhysicalDeviceFeatures physicalDeviceFeatures{};
        VkSurfaceKHR surface;
        QueueFamilyIndices queueFamily;

        PhysicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface): surface(surface) {
            this->physicalDevice = physicalDevice;
            vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
            vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

            queueFamily = getQueueFamilies();
        }

        /// Checks if all extensions necessary for this
        /// physical device is currently active for the instance
        [[nodiscard]] bool isSupported(std::vector<const char*> extensions) const {
            // First get the amount of extensions
            uint32_t extensionCount = 0;
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
            // Now populate a vector with the extensions
            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

            std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());
            for (const auto& extension : availableExtensions) {
                requiredExtensions.erase(extension.extensionName);
            }

            return requiredExtensions.empty();
        }

        [[nodiscard]] uint32_t getScore() const {
            uint32_t score = 0;

            // Score based on what GPU type we have
            // Discrete offers the most power
            // Integrated offers the second best power
            // The rest are just up to whatever we want to score
            switch (physicalDeviceProperties.deviceType) {
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    score += 1000;
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    score += 500;
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    score += 300;
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    score += 100;
                    break;
                default:
                    score += 50;
                    break;
            }

            // Higher max image quality (max image dimension) affects performance
            score += physicalDeviceProperties.limits.maxImageDimension2D;

            // GPU specific checks

            // Does this GPU support Geometry Shaders?
            if (!physicalDeviceFeatures.geometryShader) {
                score = 0;
            }

            // Does this GPU support tessellation
            if (!physicalDeviceFeatures.tessellationShader) {
                score = 0;
            }

            return score;
        }
    };
    struct Device {
        std::shared_ptr<PhysicalDevice> physicalDevice;
        VkDevice logicalDevice = VK_NULL_HANDLE;
        std::unique_ptr<Queue> graphicsQueue;
        std::unique_ptr<Queue> presentQueue;

        Device(const VkDeviceCreateInfo &info, const std::shared_ptr<PhysicalDevice>& physicalDevice) {
            this->physicalDevice = physicalDevice;
            vkCreateDevice(physicalDevice->physicalDevice, &info, nullptr, &logicalDevice);
            graphicsQueue = std::make_unique<Queue>(logicalDevice, physicalDevice->queueFamily.graphicsFamily.value());
            presentQueue = std::make_unique<Queue>(logicalDevice, physicalDevice->queueFamily.presentFamily.value());
        }
    };
    class VulkanContext {
    public:
        void init();
    private:
        // ==============
        //     VULKAN
        // ==============

        VkInstance m_instance = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
        std::shared_ptr<Device> m_device;
        std::shared_ptr<PhysicalDevice> m_physicalDevice;

        std::vector<const char*> m_extensions;
        std::vector<const char*> m_layers;

        // ==============
        // INITIALIZATION
        // ==============


        void createInstance();              // First step - establishes Vulkan API connection
        void createDebugMessenger();        // Set up early for debugging during setup
        void createSurface();               // Window surface needed for device selection

        void pickPhysicalDevice();          // Select suitable GPU
        void createLogicalDevice();         // Logical device and queues

        // Vulkan Rendering

        //void createSwapChain();             // Display chain
        //void createImageViews();            // Views into swapchain images
        //void createRenderPass();            // Define render pass structure

        //void createDescriptorSetLayout();   // Define descriptor layouts before pipeline
        //void createGraphicsPipeline();      // Create graphics pipeline

        //void createCommandPool();           // Create command pool (from selected queue family)
        //void createDepthResources();        // Add this for depth buffer (if needed)
        //void createFramebuffers();          // Create framebuffers for the render pass

        //void createDescriptorPool();        // Create descriptor pool
        //void createDescriptorSets();        // Allocate descriptor sets

        //void createCommandBuffers();        // Allocate command buffers
        //void createSyncObjects();           // Create semaphores and fences

        // ==============
        //      UTIL
        // ==============

        static std::vector<const char*> getRequiredExtensions();
    };

}

#endif //VULKANCONTEXT_H
