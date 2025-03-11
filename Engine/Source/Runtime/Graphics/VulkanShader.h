//
// Created by lepag on 3/11/25.
//

#ifndef VULKANSHADER_H
#define VULKANSHADER_H

#include <vulkan/vulkan.hpp>
#include <__filesystem/filesystem_error.h>

#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <memory>

namespace Trin::Runtime::Graphics {
    using namespace vk;

    class VertexLayout {
    public:
        // Add attribute methods
        VertexLayout& addAttribute(uint32_t location, Format format, uint32_t offset) {
            VertexInputAttributeDescription attribute;
            attribute.binding = 0;  // Assume single binding for simplicity
            attribute.location = location;
            attribute.format = format;
            attribute.offset = offset;
        
            m_attributes.push_back(attribute);
            return *this;
        }
    
        // Get descriptions for pipeline creation
        VertexInputBindingDescription getBindingDescription() const {
            return m_binding;
        }
    
        const std::vector<VertexInputAttributeDescription>& getAttributeDescriptions() const {
            return m_attributes;
        }
    
        // Initialize with stride and input rate
        VertexLayout(uint32_t stride, VertexInputRate inputRate = VertexInputRate::eVertex) {
            m_binding.binding = 0;
            m_binding.stride = stride;
            m_binding.inputRate = inputRate;
        }
    
    private:
        VertexInputBindingDescription m_binding;
        std::vector<VertexInputAttributeDescription> m_attributes;
    };

    struct ShaderInfo {
        Device device;
        ShaderStageFlagBits stage;
        std::string path;
    };

    class VulkanShader {
    public:
        VulkanShader(const ShaderInfo &info);

        ~VulkanShader();

        PipelineShaderStageCreateInfo getGraphicsCreateInfo() const;

        bool reload();

        void destroy();

    private:
        // ==============
        //     VULKAN
        // ==============

        Device m_device;

        // ==============
        //     SHADER
        // ==============

        ShaderModule m_shaderModule; /// Shader's module
        ShaderStageFlagBits m_stage; /// Properties for shader
        std::string m_path; /// Path to shader
        std::string m_entry; /// Usually "main"
        std::filesystem::file_time_type m_lastModifiedTime; /// Last time file was modified

        // ==============
        //     HELPER
        // ==============

        bool compileShader();

        // Read SPIR-V binary file
        static std::vector<uint32_t> readSPIRVFile(const std::string &filename) {
            std::ifstream file(filename, std::ios::binary | std::ios::ate);

            if (!file.is_open()) {
                throw std::runtime_error("Failed to open shader file: " + filename);
            }

            size_t fileSize = static_cast<size_t>(file.tellg());
            if (fileSize % sizeof(uint32_t) != 0) {
                throw std::runtime_error("SPIR-V file size is not a multiple of 4 bytes");
            }

            std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

            file.seekg(0);
            file.read(reinterpret_cast<char *>(buffer.data()), fileSize);
            file.close();

            return buffer;
        }

        // Read text file (for GLSL)
        static std::string readTextFile(const std::string &filename) {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);

            if (!file.is_open()) {
                throw std::runtime_error("Failed to open shader file: " + filename);
            }

            const size_t fileSize = file.tellg();
            std::string buffer(fileSize, ' ');

            file.seekg(0);
            file.read(buffer.data(), fileSize);
            file.close();

            return buffer;
        }

        // Compile GLSL to SPIR-V using glslang
        std::vector<uint32_t> compileGLSLToSPIRV(const std::string &source, std::string &errorLog) const {
            // Map Vulkan-Hpp shader stage to glslang shader stage
            EShLanguage glslangStage;
            switch (m_stage) {
                case ShaderStageFlagBits::eVertex:
                    glslangStage = EShLangVertex;
                    break;
                case ShaderStageFlagBits::eFragment:
                    glslangStage = EShLangFragment;
                    break;
                case ShaderStageFlagBits::eCompute:
                    glslangStage = EShLangCompute;
                    break;
                // Add more stages as needed
                default:
                    errorLog = "Unsupported shader stage";
                    return {};
            }

            // Create shader object
            glslang::TShader shader(glslangStage);
            const char *shaderStrings[1] = {source.c_str()};
            const int shaderLengths[1] = {static_cast<int>(source.length())};
            const char *shaderNames[1] = {m_path.c_str()};

            shader.setStringsWithLengthsAndNames(shaderStrings, shaderLengths, shaderNames, 1);

            // Set up preprocessor
            shader.setEnvInput(glslang::EShSourceGlsl, glslangStage, glslang::EShClientVulkan, 100);
            shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_2);
            shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);

            // Define resource limits
            TBuiltInResource resources = getDefaultResources();

            // Parse the shader
            if (!shader.parse(&resources, 100, false, EShMsgDefault)) {
                errorLog = shader.getInfoLog();
                return {};
            }

            // Link program
            glslang::TProgram program;
            program.addShader(&shader);

            if (!program.link(EShMsgDefault)) {
                errorLog = program.getInfoLog();
                return {};
            }

            // Generate SPIR-V
            std::vector<uint32_t> spirvCode;
            glslang::SpvOptions spvOptions;
            spvOptions.generateDebugInfo = true;
            spvOptions.optimizeSize = false;
            spvOptions.disableOptimizer = false;

            GlslangToSpv(*program.getIntermediate(glslangStage), spirvCode, &spvOptions);

            return spirvCode;
        }

        // Default resource limits for glslang
        static TBuiltInResource getDefaultResources() {
            TBuiltInResource resources = {};

            resources.maxLights = 32;
            resources.maxClipPlanes = 6;
            resources.maxTextureUnits = 32;
            resources.maxTextureCoords = 32;
            resources.maxVertexAttribs = 64;
            resources.maxVertexUniformComponents = 4096;
            resources.maxVaryingFloats = 64;
            resources.maxVertexTextureImageUnits = 32;
            resources.maxCombinedTextureImageUnits = 80;
            resources.maxTextureImageUnits = 32;
            resources.maxFragmentUniformComponents = 4096;
            resources.maxDrawBuffers = 32;
            resources.maxVertexUniformVectors = 128;
            resources.maxVaryingVectors = 8;
            resources.maxFragmentUniformVectors = 16;
            resources.maxVertexOutputVectors = 16;
            resources.maxFragmentInputVectors = 15;
            resources.minProgramTexelOffset = -8;
            resources.maxProgramTexelOffset = 7;
            resources.maxClipDistances = 8;
            resources.maxComputeWorkGroupCountX = 65535;
            resources.maxComputeWorkGroupCountY = 65535;
            resources.maxComputeWorkGroupCountZ = 65535;
            resources.maxComputeWorkGroupSizeX = 1024;
            resources.maxComputeWorkGroupSizeY = 1024;
            resources.maxComputeWorkGroupSizeZ = 64;
            resources.maxComputeUniformComponents = 1024;
            resources.maxComputeTextureImageUnits = 16;
            resources.maxComputeImageUniforms = 8;
            resources.maxComputeAtomicCounters = 8;
            resources.maxComputeAtomicCounterBuffers = 1;
            resources.maxVaryingComponents = 60;
            resources.maxVertexOutputComponents = 64;
            resources.maxGeometryInputComponents = 64;
            resources.maxGeometryOutputComponents = 128;
            resources.maxFragmentInputComponents = 128;
            resources.maxImageUnits = 8;
            resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
            resources.maxCombinedShaderOutputResources = 8;
            resources.maxImageSamples = 0;
            resources.maxVertexImageUniforms = 0;
            resources.maxTessControlImageUniforms = 0;
            resources.maxTessEvaluationImageUniforms = 0;
            resources.maxGeometryImageUniforms = 0;
            resources.maxFragmentImageUniforms = 8;
            resources.maxCombinedImageUniforms = 8;
            resources.maxGeometryTextureImageUnits = 16;
            resources.maxGeometryOutputVertices = 256;
            resources.maxGeometryTotalOutputComponents = 1024;
            resources.maxGeometryUniformComponents = 1024;
            resources.maxGeometryVaryingComponents = 64;
            resources.maxTessControlInputComponents = 128;
            resources.maxTessControlOutputComponents = 128;
            resources.maxTessControlTextureImageUnits = 16;
            resources.maxTessControlUniformComponents = 1024;
            resources.maxTessControlTotalOutputComponents = 4096;
            resources.maxTessEvaluationInputComponents = 128;
            resources.maxTessEvaluationOutputComponents = 128;
            resources.maxTessEvaluationTextureImageUnits = 16;
            resources.maxTessEvaluationUniformComponents = 1024;
            resources.maxTessPatchComponents = 120;
            resources.maxPatchVertices = 32;
            resources.maxTessGenLevel = 64;
            resources.maxViewports = 16;
            resources.maxVertexAtomicCounters = 0;
            resources.maxTessControlAtomicCounters = 0;
            resources.maxTessEvaluationAtomicCounters = 0;
            resources.maxGeometryAtomicCounters = 0;
            resources.maxFragmentAtomicCounters = 8;
            resources.maxCombinedAtomicCounters = 8;
            resources.maxAtomicCounterBindings = 1;
            resources.maxVertexAtomicCounterBuffers = 0;
            resources.maxTessControlAtomicCounterBuffers = 0;
            resources.maxTessEvaluationAtomicCounterBuffers = 0;
            resources.maxGeometryAtomicCounterBuffers = 0;
            resources.maxFragmentAtomicCounterBuffers = 1;
            resources.maxCombinedAtomicCounterBuffers = 1;
            resources.maxAtomicCounterBufferSize = 16384;
            resources.maxTransformFeedbackBuffers = 4;
            resources.maxTransformFeedbackInterleavedComponents = 64;
            resources.maxCullDistances = 8;
            resources.maxCombinedClipAndCullDistances = 8;
            resources.maxSamples = 4;

            // Set all the limits fields
            resources.limits.nonInductiveForLoops = 1;
            resources.limits.whileLoops = 1;
            resources.limits.doWhileLoops = 1;
            resources.limits.generalUniformIndexing = 1;
            resources.limits.generalAttributeMatrixVectorIndexing = 1;
            resources.limits.generalVaryingIndexing = 1;
            resources.limits.generalSamplerIndexing = 1;
            resources.limits.generalVariableIndexing = 1;
            resources.limits.generalConstantMatrixVectorIndexing = 1;

            return resources;
        }
    };
}

#endif //VULKANSHADER_H
