//
// Created by lepag on 3/11/25.
//

#ifndef VULKANPROGRAM_H
#define VULKANPROGRAM_H

#include <vulkan/vulkan.hpp>
#include "VulkanShader.h"

namespace Trin::Runtime::Graphics {

    class VulkanProgram {
    public:
        VulkanProgram(Device device,
                      const std::string& vertexShaderPath,
                      const std::string& fragmentShaderPath);
    
        // Check for shader changes and reload if necessary
        bool checkForChanges() const;
    
        // Get shader stages for pipeline creation
        std::vector<PipelineShaderStageCreateInfo> getShaderStages() const;
    
    private:
        Device m_device;
        std::unique_ptr<VulkanShader> m_vertexShader;
        std::unique_ptr<VulkanShader> m_fragmentShader;
    };

}

#endif //VULKANPROGRAM_H
