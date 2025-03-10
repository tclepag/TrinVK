//
// Created by lepag on 3/9/2025.
//

#ifndef VULKANCMD_H
#define VULKANCMD_H

#include <vulkan/vulkan.hpp>

namespace Trin::Runtime::Rendering::Commands {
    using namespace vk;
class VulkanCMDS {
public:
    VulkanCMDS();
    ~VulkanCMDS();

    void init();
    void begin();
private:
    CommandBufferBeginInfo m_beginInfo;
};

}

#endif //VULKANCMD_H
