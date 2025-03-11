//
// Created by lepag on 3/10/25.
//

#ifndef VULKANSHADER_H
#define VULKANSHADER_H

#include <string>
#include <array>
#include <stdexcept>
#include <vulkan/vulkan.hpp>

#include "Core/VulkanCore.h"

namespace Trin::Runtime::Rendering {
    using namespace Core;
    using namespace vk;

    enum class ShaderType {
        Vertex,
        Fragment,
    };

    using ShaderStage = std::array<PipelineShaderStageCreateInfo, 2>;

    class VulkanShader {
    public:
        explicit VulkanShader(VulkanCore* core);
        ~VulkanShader();

        /// Sets the path to the fragment shader
        void setFragmentPath(const std::string& filePath);

        /// Sets the path to the vertex shader
        void setVertexPath(const std::string& filePath);

        /// Builds the fragment and vertex shaders for this VulkanShader
        /// @return Array of shader stage create info objects for pipeline creation
        ShaderStage build();

        /// Destroys the shader modules
        void cleanup();

        /// Returns vertex shader module
        [[nodiscard]] ShaderModule getVertexShader() const {
            return m_vertShaderModule;
        }

        /// Returns fragment shader module
        [[nodiscard]] ShaderModule getFragmentShader() const {
            return m_fragShaderModule;
        }

        /// Returns true if both shader modules are valid
        [[nodiscard]] bool isValid() const {
            return static_cast<bool>(m_vertShaderModule) &&
                   static_cast<bool>(m_fragShaderModule);
        }

    private:
        // Core vulkan
        VulkanCore* m_core;
        Device m_device;
        ShaderModule m_vertShaderModule;
        ShaderModule m_fragShaderModule;

        // Shader paths
        std::string m_fragmentPath;
        std::string m_vertexPath;

        // Cached shader stages (built during build())
        ShaderStage m_shaderStages;

        /// Loads a shader module from the specified file
        /// @param filePath Path to the shader file
        /// @param type Type of shader (vertex or fragment)
        /// @return ShaderModule object
        /// @throws std::runtime_error if shader loading fails
        ShaderModule loadShader(const std::string& filePath, ShaderType type);

        /// Creates a shader module from binary code
        /// @param code Vector containing the shader binary code
        /// @return ShaderModule object
        /// @throws std::runtime_error if shader module creation fails
        ShaderModule createShaderModule(const std::vector<char>& code) const;
    };
}

#endif //VULKANSHADER_H