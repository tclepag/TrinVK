//
// Created by lepag on 3/11/25.
//

#include "VulkanProgram.h"

namespace Trin::Runtime::Graphics {
    VulkanProgram::VulkanProgram(const Device device,
        const std::string &vertexShaderPath,
        const std::string &fragmentShaderPath): m_device(device) {

        ShaderInfo vertexShaderInfo{};
        vertexShaderInfo.device = device;
        vertexShaderInfo.path = vertexShaderPath;
        vertexShaderInfo.stage = ShaderStageFlagBits::eVertex;

        m_vertexShader = std::make_unique<VulkanShader>(vertexShaderInfo);

        ShaderInfo fragmentShaderInfo{};
        fragmentShaderInfo.device = device;
        fragmentShaderInfo.path = fragmentShaderPath;
        fragmentShaderInfo.stage = ShaderStageFlagBits::eFragment;

        m_fragmentShader = std::make_unique<VulkanShader>(fragmentShaderInfo);
    }

    bool VulkanProgram::checkForChanges() const {
        return m_vertexShader->reload() || m_fragmentShader->reload();
    }

    std::vector<PipelineShaderStageCreateInfo> VulkanProgram::getShaderStages() const {
        std::vector<PipelineShaderStageCreateInfo> stages;
        stages.push_back(m_vertexShader->getGraphicsCreateInfo());
        stages.push_back(m_fragmentShader->getGraphicsCreateInfo());
        return stages;
    }


}
