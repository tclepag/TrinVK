//
// Created by lepag on 3/10/2025.
//

#include "VulkanContext.h"
#include "Window.h"
#include <set>


namespace Trin::Runtime::Core {
    VulkanContext::VulkanContext(): m_instance(nullptr),
                                    m_device(nullptr), m_window(nullptr),
                                    m_physicalDevice(nullptr),
                                    m_graphicsQueue(nullptr),
                                    m_presentQueue(nullptr),
                                    m_apiVersion(0), m_applicationName(nullptr), m_enableValidationLayers(false),
                                    m_debugger(nullptr) {
    }

    VulkanContext::~VulkanContext() {
        cleanup();
    }

    void VulkanContext::init(const VulkanCreateInfo& info) {
        // Setup variables
        this->m_apiVersion = info.applicationVersion;
        this->m_enableValidationLayers = info.enableValidationLayers;
        this->m_applicationName = info.applicationName;
        this->m_window = info.window;

        // Initialize GLSlang
        if (!glslang::InitializeProcess()) {
            std::cerr << "Failed to initialize GLSLang!" << std::endl;
            throw std::runtime_error("Failed to initialize GLSLang!");
        }

        // Initialize Vulkan
        initInstance();
        createDebugUtilsMessengerEXT();
        initSurface();
        initPhysicalDevice();
        initDevice();
        initQueues();
    }

    void VulkanContext::cleanup() {
        // Vulkan Cleanup goes in reverse
        if (m_device) {
            // Wait on device to finish operations
            m_device.waitIdle();
            m_device.destroy();
            m_device = nullptr;
        }

        if (m_instance && m_surface) {
            m_instance.destroySurfaceKHR(m_surface);
            m_surface = nullptr;
        }

        if (m_enableValidationLayers && m_instance && m_debugger) {
            destroyDebugUtilsMessengerEXT();
            m_debugger = nullptr;
        }

        if (m_instance) {
            m_instance.destroy();
            m_instance = nullptr;
        }

        glslang::FinalizeProcess();
    }

    void VulkanContext::initInstance() {
        // Setup information of application
        ApplicationInfo appInfo = {};
        appInfo.pApplicationName = m_applicationName;
        appInfo.applicationVersion = m_apiVersion;
        appInfo.pEngineName = Constants::ENGINE_NAME;
        appInfo.engineVersion = Constants::ENGINE_VERSION;
        appInfo.apiVersion = Constants::ENGINE_VERSION;

        auto extensions = getRequiredExtensions();

        // Setup information for the Vulkan Instance to build from
        InstanceCreateInfo createInfo = {};
        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.pApplicationInfo = &appInfo;

#ifdef __APPLE__
        createInfo.flags |= InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

        // Build our instance
        m_instance = createInstance(createInfo);
    }

    void VulkanContext::initDevice() {
        QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);
        std::vector<DeviceQueueCreateInfo> queueCreateInfos;

        std::set queueFamilyIndices = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value(),
        };

        float queuePriority = 1.0f;

        for (uint32_t queueFamily : queueFamilyIndices) {
            DeviceQueueCreateInfo createInfo{};
            createInfo.queueFamilyIndex = queueFamily;
            createInfo.queueCount = 1;
            createInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(createInfo);
        }

        PhysicalDeviceFeatures deviceFeatures;

        // Engine Features
        deviceFeatures.samplerAnisotropy = true;
        deviceFeatures.fillModeNonSolid = true;

        std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        // Add macOS-specific device extensions
#ifdef __APPLE__
        deviceExtensions.push_back("VK_KHR_portability_subset");
#endif

        DeviceCreateInfo createInfo{};
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        try {
            m_device = m_physicalDevice.createDevice(createInfo);
            std::cout << "Logical device created successfully" << std::endl;
        } catch (SystemError& err) {
            throw std::runtime_error("Failed to create logical device: " + std::string(err.what()));
        }
    }

    void VulkanContext::initSurface() {
        VkSurfaceKHR surface = m_window->createSurface(this->getInstance());
        m_surface = SurfaceKHR(surface);
    }

    void VulkanContext::initPhysicalDevice() {
        std::vector<PhysicalDevice> physicalDevices = m_instance.enumeratePhysicalDevices();
        if (physicalDevices.empty()) {
            throw std::runtime_error("failed to enumerate physical devices");
        }

        std::vector<RankedPhysicalDevice> rankedDevices{};
        for (const auto& device : physicalDevices) {
            RankedPhysicalDevice physical{};
            physical.physicalDevice = device;
            rankedDevices.emplace_back(physical);
            rateDevice(device);
        }

        std::ranges::sort(rankedDevices,
                          [](const RankedPhysicalDevice& a, const RankedPhysicalDevice& b) {
                              return a.score > b.score;
                          });

        if (rankedDevices.empty()) {
            throw std::runtime_error("Failed to find a suitable GPU");
        }

        // Select the best device
        m_physicalDevice = rankedDevices[0].physicalDevice;

        // Print selected device info
        PhysicalDeviceProperties deviceProperties = m_physicalDevice.getProperties();
        std::cout << "Selected GPU: " << deviceProperties.deviceName << std::endl;
        std::cout << "API Version: " << VK_VERSION_MAJOR(deviceProperties.apiVersion) << "."
                                     << VK_VERSION_MINOR(deviceProperties.apiVersion) << "."
                                     << VK_VERSION_PATCH(deviceProperties.apiVersion) << std::endl;
    }

    void VulkanContext::initQueues() {
        QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

        // Get the graphics queue handle
        m_graphicsQueue = m_device.getQueue(indices.graphicsFamily.value(), 0);

        // Get the present queue handle
        m_presentQueue = m_device.getQueue(indices.presentFamily.value(), 0);

        // Get the transfer queue handle
        m_transferQueue = m_device.getQueue(indices.transferFamily.value(), 0);

        std::cout << "Queue handles retrieved successfully" << std::endl;
    }

    QueueFamilyIndices VulkanContext::findQueueFamilies(const PhysicalDevice &physicalDevice) {
        QueueFamilyIndices indices;
        std::vector<QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

        for (uint32_t i =0; i < queueFamilies.size(); i++) {
            // Check for the physicalDevice's graphics queue family
            if (queueFamilies[i].queueFlags & QueueFlagBits::eGraphics) {
                indices.graphicsFamily = i;

                // If we don't have a presentation family, we just use the graphics family
                if (!indices.presentFamily.has_value()) {
                    indices.presentFamily = i;
                }

                // If both graphics and presentation families were found, then we can stop
                if (indices.isComplete()) {
                    break;
                }
            }
        }

        for (uint32_t i = 0; i < queueFamilies.size(); i++) {
            const auto& queueFamily = queueFamilies[i];

            // Check if this queue family supports transfer operations but not graphics
            // This would be ideal for transfer-only operations
            if (queueFamily.queueFlags & QueueFlagBits::eTransfer &&
                !(queueFamily.queueFlags & QueueFlagBits::eGraphics)) {
                indices.transferFamily = i;
                }
        }

        // If no dedicated transfer queue is found, fall back to any queue with transfer support
        for (uint32_t i = 0; i < queueFamilies.size(); i++) {
            const auto& queueFamily = queueFamilies[i];

            // Check if this queue family supports transfer operations
            if (queueFamily.queueFlags & QueueFlagBits::eTransfer) {
                indices.transferFamily = i;
            }
        }

        // If we couldn't find a presentation family for the first loop
        // we will search for it
        if (!indices.presentFamily.has_value()) {
            for (uint32_t i =0; i < queueFamilies.size(); i++) {
                // Checks if this family queue supports presentation for our surface
                if (physicalDevice.getSurfaceSupportKHR(i, m_surface)) {
                    indices.presentFamily = i;
                    break;
                }
            }
        }
        return indices;
    }

    bool VulkanContext::isDeviceSuitable(const PhysicalDevice &physicalDevice) {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        bool supported = checkDeviceExtensionSupport(physicalDevice);
        bool adequate = false;
        if (supported) {
            adequate = getSwapChainSupport(physicalDevice).isAdequate();
        }

        // Get Device features and add them to the return for specific features required

        return indices.isComplete() && supported && adequate;
    }

    uint32_t VulkanContext::rateDevice(const PhysicalDevice &physicalDevice) {
        const PhysicalDeviceFeatures physicalDeviceFeatures = physicalDevice.getFeatures();
        const PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();

        uint32_t score = 0;

        // Check if device meets our standards
        if (!isDeviceSuitable(physicalDevice)) {
            return 0; // Not suitable, set to 0
        }

        // Choose Discrete GPUs over Integrated as they are very powerful
        if (physicalDeviceProperties.deviceType == PhysicalDeviceType::eDiscreteGpu) {
            score += 1000;
        } else if (physicalDeviceProperties.deviceType == PhysicalDeviceType::eIntegratedGpu) {
            score += 500;
        }

        // Higher maximum image dimensions mean better rendering
        score += physicalDeviceProperties.limits.maxImageDimension2D;

        // Other specifics that make this physicalDevice priority
        if (physicalDeviceFeatures.geometryShader) {
            score += 100;
        }

        if (physicalDeviceFeatures.tessellationShader) {
            score += 50;
        }

        if (physicalDeviceFeatures.multiViewport) {
            score += 50;
        }

        return score;
    }

    void VulkanContext::createDebugUtilsMessengerEXT() {
        DebugUtilsMessengerCreateInfoEXT createInfo{};
        // Set message severities we will accept for our callback
        createInfo.setMessageSeverity(
            DebugUtilsMessageSeverityFlagBitsEXT::eError |
            DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
        );
        // Set message types we will accept for our callback
        createInfo.setMessageType(
            DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            DebugUtilsMessageTypeFlagBitsEXT::eValidation
        );

        createInfo.setPfnUserCallback(&VulkanContext::debugCallback);

        // Get the extension to create a debugger
        auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
            m_instance, "vkCreateDebugUtilsMessengerEXT"));

        if (func != nullptr) {
            VkDebugUtilsMessengerEXT debugMessenger;
            VkResult result = func(m_instance,
                                  reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo),
                                  nullptr,
                                  &debugMessenger);

            if (result == VK_SUCCESS) {
                m_debugger = debugMessenger;
            } else {
                throw std::runtime_error("Failed to set up debug messenger");
            }
        } else {
            throw std::runtime_error("Failed to find vkCreateDebugUtilsMessengerEXT function");
        }
    }

    void VulkanContext::destroyDebugUtilsMessengerEXT() const {
        if (!m_enableValidationLayers || !m_debugger) return;

        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
            m_instance, "vkDestroyDebugUtilsMessengerEXT"));

        if (func != nullptr) {
            func(m_instance, m_debugger, nullptr);
        }
    }

    bool VulkanContext::isValidationLayersSupported() {
        // Get all available validation layers
        std::vector<LayerProperties> availableLayers = enumerateInstanceLayerProperties();

        // Define the validation layers we want to enable
        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"  // This meta-layer includes most validation functionality
        };

        // Check if each requested layer is available
        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                std::cerr << "Validation layer " << layerName << " not available!" << std::endl;
                return false;
            }
        }

        return true;
    }

    std::vector<const char*> VulkanContext::getRequiredExtensions() const {
        std::vector <const char*> extensions;

        // Add GLFW extensions
        uint32_t extensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
        for (uint32_t i = 0; i < extensionCount; i++) {
            extensions.push_back(glfwExtensions[i]);
        }

#ifdef __APPLE__
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#endif

        // If validation layers are enabled, add the debug extension
        if (m_enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool VulkanContext::checkDeviceExtensionSupport(const PhysicalDevice &physicalDevice) {
        // Get extensions supported by the device
        std::vector<ExtensionProperties> availableExtensions =
            physicalDevice.enumerateDeviceExtensionProperties();

        // Define required device extensions
        const std::vector deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        // Create a set of required extensions (easier for checking)
        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        // For each available extension, remove it from the required set if found
        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        // If the set is empty, all required extensions are supported
        return requiredExtensions.empty();
    }

    SwapChainSupportDetails VulkanContext::getSwapChainSupport(const PhysicalDevice &physicalDevice) const {
        SwapChainSupportDetails details;

        // Get the surface capabilities
        details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(m_surface);

        // Get the supported surface formats
        details.formats = physicalDevice.getSurfaceFormatsKHR(m_surface);

        // Get the supported presentation modes
        details.presentModes = physicalDevice.getSurfacePresentModesKHR(m_surface);

        return details;
    }
}
