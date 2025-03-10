//
// Created by lepag on 3/9/25.
//

#include "VulkanCore.h"


namespace Trin::Runtime::Core {
    VulkanCore::VulkanCore(): m_instance(nullptr),
                              m_physicalDevice(nullptr),
                              m_device(nullptr),
                              m_surface(nullptr),
                              m_graphicsQueue(nullptr),
                              m_presentQueue(nullptr),
                              m_debugMessenger(nullptr) {
    }

    VulkanCore::~VulkanCore() = default;

    bool VulkanCore::initialize(VulkanInitializationInfo &info) {
        // Append all core extension + layers to the info
        for (auto extension: info.extensionNames) {
            extensionNames.emplace_back(extension);
        }
        for (auto layer: info.layerNames) {
            layerNames.emplace_back(layer);
        }
        // Get values from tuple
        auto [f, s, t] = info.appVersion;
        // Setup Application Info for Vulkan
        ApplicationInfo appInfo{};
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, f, s, t);
        appInfo.pApplicationName = info.appName.c_str();
        appInfo.pEngineName = info.engineName.c_str();

        // Create initialization info for Vulkan
        InstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.pApplicationInfo = &appInfo;
        instanceCreateInfo.setPEnabledLayerNames(layerNames);
        instanceCreateInfo.setPEnabledExtensionNames(extensionNames);

        // Create Vulkan Instance
        m_instance = createInstance(instanceCreateInfo);

        if (m_instance == VK_NULL_HANDLE) {
            System::PostFatalError(
                "VULKAN INITIALIZATION ERROR",
                "VULKAN INSTANCE FAILED TO CREATE"
            );
            return false;
        }

        // Create debug messenger
        if (isExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
            DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            // Set message levels we are allowing for our debugCallback
            debugCreateInfo.setMessageSeverity(
                DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                DebugUtilsMessageSeverityFlagBitsEXT::eError
            );

            // Set message types we are allowing for our debugCallback
            debugCreateInfo.setMessageType(
                DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                DebugUtilsMessageTypeFlagBitsEXT::eValidation
            );

            // Create the debugMessenger
            m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(debugCreateInfo);
        }

        if (!info.window) {
            System::PostFatalError(
                "VULKAN INITIALIZATION ERROR",
                "NO WINDOW WAS PROVIDED"
            );
            return false;
        }

        // Create VkSurface
        VkSurfaceKHR vkSurface = VK_NULL_HANDLE;
        SDL_Vulkan_CreateSurface(info.window->GetWindow(), static_cast<VkInstance>(m_instance), nullptr, &vkSurface);
        // Convert VkSurfaceKHR to C++ binding version
        m_surface = SurfaceKHR(vkSurface);

        // Get the best Physical Device for Vulkan
        m_physicalDevice = getBestDevice();

        // Create the logical device
        QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

        // Create a set of unique queue families needed
        std::set<uint32_t> uniqueQueueFamilies = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value()
        };

        // Create queue create infos for each unique queue family
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        float queuePriority = 1.0f;

        for (uint32_t queueFamily : uniqueQueueFamilies) {
            vk::DeviceQueueCreateInfo queueCreateInfo;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // Define device features to enable
        vk::PhysicalDeviceFeatures deviceFeatures;
        // Enable specific features you need
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        // Create the logical device
        DeviceCreateInfo createInfo;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());
        createInfo.ppEnabledExtensionNames = extensionNames.data();

        try {
            m_device = m_physicalDevice.createDevice(createInfo);

            // Get queue handles
            m_graphicsQueue = m_device.getQueue(indices.graphicsFamily.value(), 0);
            m_presentQueue = m_device.getQueue(indices.presentFamily.value(), 0);

            std::cout << "Logical device created successfully\n";
        } catch (vk::SystemError& err) {
            throw std::runtime_error("Failed to create logical device: " + std::string(err.what()));
        }

        return true;
    }

    bool VulkanCore::isExtensionSupported(const String &extension) {
        std::vector<ExtensionProperties> availableExtensions = enumerateInstanceExtensionProperties();
        return std::ranges::any_of(availableExtensions, [&extension](const auto &vkExtension) {
            return vkExtension.extensionName == extension;
        });
    }

    bool VulkanCore::cleanup() const {
        if (m_device) {
            m_device.destroy();
        }

        if (m_debugMessenger) {
            m_instance.destroyDebugUtilsMessengerEXT(m_debugMessenger);
        }

        if (m_surface) {
            m_instance.destroySurfaceKHR(m_surface);
        }

        if (m_instance) {
            m_instance.destroy();
        }
        return true;
    }

    bool VulkanCore::isDeviceSupported(const PhysicalDevice &physicalDevice) {
        std::vector<ExtensionProperties> availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();

        // Create a set of required extensions
        std::set<std::string> requiredExtensions(extensionNames.begin(), extensionNames.end());

        // Remove each found extension from the required set
        for (const auto &extension: availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        // If the set is empty, all required extensions are supported
        return requiredExtensions.empty();
    }

    QueueFamilyIndices VulkanCore::findQueueFamilies(const PhysicalDevice &physicalDevice) {
        QueueFamilyIndices indices;

        // Get all queue family properties
        std::vector<QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

        // Find graphics queue family
        for (uint32_t i = 0; i < queueFamilies.size(); i++) {
            if (queueFamilies[i].queueFlags & QueueFlagBits::eGraphics) {
                indices.graphicsFamily = i;
            }

            // Check presentation support
            if (physicalDevice.getSurfaceSupportKHR(i, m_surface)) {
                indices.presentFamily = i;
            }

            // Exit early if we found all required queue families
            if (indices.isComplete()) {
                break;
            }
        }

        return indices;
    }

    PhysicalDevice VulkanCore::getBestDevice() {
        // Get every GPU device from the computer
        std::vector<PhysicalDevice> physicalDevices = m_instance.enumeratePhysicalDevices();
        if (physicalDevices.empty()) {
            System::PostFatalError(
                "VULKAN PHYSICAL DEVICE ERROR",
                "NO SUITABLE GPU FOUND"
            );
            throw std::runtime_error("Failed to find a suitable GPU!");
        }

        // Start ranking devices to see which is the most suitable for Vulkan
        std::vector<PhysicalDeviceDetails> rankedDevices = {};
        for (const auto &device: physicalDevices) {
            // Check to see if the device is suitable to start scoring
            // If the GPU has no QueueFamily or isn't fully supported by our extension list
            // then we ignore it
            QueueFamilyIndices indices = findQueueFamilies(device);
            bool extensionsSupported = isDeviceSupported(device);
            if (!indices.isComplete() || !extensionsSupported) { continue; }

            uint32_t score = 0;
            // Get device features
            PhysicalDeviceProperties deviceProperties = device.getProperties();
            PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

            // Rate GPU based on GPU type
            // We try to prefer Discrete as much as possible since they provide alot of power.
            //
            // Integrated GPUs are built into the pc, like a laptop's GPU.
            // They can be pretty powerful, but discrete is usually always more powerful
            if (deviceProperties.deviceType == PhysicalDeviceType::eDiscreteGpu) {
                score += 1000;
            } else if (deviceProperties.deviceType == PhysicalDeviceType::eIntegratedGpu) {
                score += 500;
            }

            // We also rate GPUs based on their maximum image dimensions
            // Basically in short, larger image dimensions mean a more
            // powerful GPU while smaller dimensions means the GPU isn't very powerful
            score += deviceProperties.limits.maxImageDimension2D;

            // If the GPU doesn't support geometry shaders we avoid it
            if (!deviceFeatures.geometryShader) {
                continue;
            }

            // If we made it here then that means that the GPU is suitable for ranking
            PhysicalDeviceProperties props = device.getProperties();

            // Store device details
            PhysicalDeviceDetails details;
            details.device = device;
            details.name = std::string(props.deviceName);
            details.type = props.deviceType;
            details.score = score;

            rankedDevices.push_back(details);
        }

        // If no GPU was found to be suitable for ranking then give up
        // But this it's probably a nil chance it'll happen
        if (rankedDevices.empty()) {
            System::PostFatalError(
                "VULKAN PHYSICAL DEVICE ERROR",
                "NO SUITABLE GPUS FOR RANKING"
            );
            throw std::runtime_error("Failed to find a suitable GPU!");
        }

        // Use ranges to sort the physicalDevices from most score to least
        std::ranges::sort(rankedDevices,
                          [](const PhysicalDeviceDetails &a, const PhysicalDeviceDetails &b) {
                              return a.score > b.score;
                          });

        // Log all available Vulkan Devices for debugging
        std::cout << "Available Vulkan Devices:\n";
        for (const auto& device : rankedDevices) {
            std::string typeStr;
            switch (device.type) {
                case vk::PhysicalDeviceType::eDiscreteGpu: typeStr = "Discrete GPU"; break;
                case vk::PhysicalDeviceType::eIntegratedGpu: typeStr = "Integrated GPU"; break;
                case vk::PhysicalDeviceType::eVirtualGpu: typeStr = "Virtual GPU"; break;
                case vk::PhysicalDeviceType::eCpu: typeStr = "CPU"; break;
                default: typeStr = "Other";
            }

            std::cout << "  - " << device.name << " (" << typeStr << "), Score: " << device.score << "\n";
        }

        // Return the first ranked device as it is the best suited for Vulkan
        return rankedDevices[0].device;
    }

    Bool32 VulkanCore::debugCallback(DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                 DebugUtilsMessageTypeFlagsEXT messageType,
                                 const DebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                 void *pUserData) {
        std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }
}
