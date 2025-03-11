//
// Created by lepag on 3/10/25.
//

#include "VulkanShader.h"
#include "Helpers/File.h"
#include <iostream>

namespace Trin::Runtime::Rendering {
    VulkanShader::VulkanShader(VulkanCore *core) :
        m_core(core),
        m_device(core->getDevice()),
        m_vertShaderModule(nullptr),
        m_fragShaderModule(nullptr) {
    }

    VulkanShader::~VulkanShader() {
        cleanup();
    }

    void VulkanShader::setFragmentPath(const std::string& filePath) {
        m_fragmentPath = filePath;
    }

    void VulkanShader::setVertexPath(const std::string& filePath) {
        m_vertexPath = filePath;
    }

    void VulkanShader::cleanup() {
        // Only destroy modules if they are valid
        if (m_vertShaderModule) {
            m_device.destroyShaderModule(m_vertShaderModule);
            m_vertShaderModule = nullptr;
        }

        if (m_fragShaderModule) {
            m_device.destroyShaderModule(m_fragShaderModule);
            m_fragShaderModule = nullptr;
        }
    }

    ShaderStage VulkanShader::build() {
        // Clean up any existing shader modules first
        cleanup();

        try {
            // Load shader modules
            m_vertShaderModule = loadShader(m_vertexPath, ShaderType::Vertex);
            m_fragShaderModule = loadShader(m_fragmentPath, ShaderType::Fragment);

            // Setup vertex shader stage
            PipelineShaderStageCreateInfo vertShaderStageInfo{};
            vertShaderStageInfo.stage = ShaderStageFlagBits::eVertex;
            vertShaderStageInfo.module = m_vertShaderModule;
            vertShaderStageInfo.pName = "main";

            // Setup fragment shader stage
            PipelineShaderStageCreateInfo fragShaderStageInfo{};
            fragShaderStageInfo.stage = ShaderStageFlagBits::eFragment;
            fragShaderStageInfo.module = m_fragShaderModule;
            fragShaderStageInfo.pName = "main";

            // Store shader stages in member array
            m_shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

            return m_shaderStages;
        }
        catch (const std::exception& e) {
            // Clean up if there was an error
            cleanup();
            throw std::runtime_error("Failed to build shader: " + std::string(e.what()));
        }
    }

    ShaderModule VulkanShader::loadShader(const std::string& filePath, ShaderType type) {
        try {
            // Read shader file
            FileObject *file(File::file(filePath.c_str(), FileType::Read));
            if (!file) {
                throw std::runtime_error("Failed to open shader file: " + filePath);
            }

            // Create vector to hold shader code
            std::vector<char> shaderCode(file->size());

            // Copy shader code to vector
            std::string text = file->text();
            std::ranges::copy(text, shaderCode.begin());

            file->close();

            // Create shader module
            return createShaderModule(shaderCode);
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Error loading shader '" + filePath + "': " + e.what());
        }
    }

    ShaderModule VulkanShader::createShaderModule(const std::vector<char>& code) const {
        // Create shader module create info
        ShaderModuleCreateInfo createInfo{};
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        try {
            // Create the shader module
            ShaderModule shaderModule = m_device.createShaderModule(createInfo);
            return shaderModule;
        }
        catch (const SystemError& e) {
            throw std::runtime_error("Failed to create shader module: " + std::string(e.what()));
        }
    }
}