//
// Created by lepag on 3/11/25.
//

#include "VulkanShader.h"

namespace Trin::Runtime::Graphics {
    VulkanShader::VulkanShader(const ShaderInfo &info) {
        m_device = info.device;
        m_path = info.path;
        m_stage = info.stage;
    }

    VulkanShader::~VulkanShader() {
        if (m_shaderModule) {
            m_device.destroyShaderModule(m_shaderModule);
        }
    }

    PipelineShaderStageCreateInfo VulkanShader::getGraphicsCreateInfo() const {
        PipelineShaderStageCreateInfo shaderStageInfo;
        shaderStageInfo.stage = m_stage;
        shaderStageInfo.module = m_shaderModule;
        shaderStageInfo.pName = m_entry.c_str();
        return shaderStageInfo;
    }

    bool VulkanShader::reload() {
        try {
            auto currentTime = std::filesystem::last_write_time(m_path);
            if (currentTime <= m_lastModifiedTime) {
                return false; // No changes
            }

            // File has changed, reload it
            ShaderModule oldModule = m_shaderModule;

            if (compileShader()) {
                // Clean up old module after successful reload
                if (oldModule) {
                    m_device.destroyShaderModule(oldModule);
                }
                std::cout << "Shader reloaded: " << m_path << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to reload shader: " << e.what() << std::endl;
        }
        return false;
    }

    void VulkanShader::destroy() {
        delete this;
    }

    bool VulkanShader::compileShader() {
        try {
            std::vector<uint32_t> spirvCode;

            // Update file modification time
            m_lastModifiedTime = std::filesystem::last_write_time(m_path);

            // Check file extension to determine how to load
            if (m_path.ends_with(".spv")) {
                // Load pre-compiled SPIR-V
                spirvCode = readSPIRVFile(m_path);
            } else {
                // Compile GLSL to SPIR-V
                std::string source = readTextFile(m_path);
                std::string errorLog;
                spirvCode = compileGLSLToSPIRV(source, errorLog);

                if (spirvCode.empty()) {
                    std::cerr << "Shader compilation failed: " << errorLog << std::endl;
                    return false;
                }
            }

            // Create shader module
            ShaderModuleCreateInfo createInfo;
            createInfo.codeSize = spirvCode.size() * sizeof(uint32_t);
            createInfo.pCode = spirvCode.data();

            m_shaderModule = m_device.createShaderModule(createInfo);
            return true;

        } catch (const std::exception& e) {
            std::cerr << "Error loading shader: " << e.what() << std::endl;
            return false;
        }
    }

}
