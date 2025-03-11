//
// Created by lepag on 3/11/25.
//

#ifndef VULKANCOMMANDBUFFER_H
#define VULKANCOMMANDBUFFER_H

#include "Graphics/VulkanProgram.h"

namespace Trin::Runtime::Core {
    using namespace Graphics;
    class VulkanCommandBuffer {
    public:
        VulkanCommandBuffer();

        ~VulkanCommandBuffer();

        // ==============
        //     STATES
        // ==============

        /// Starts recording this command buffers
        void begin();
        /// Stops recording this command buffer
        void end();
        /// Tells the command buffer to use this shader program for rendering
        void bindShaderProgram(VulkanProgram* program);
        /// Tells the command buffer to use this mesh for rendering
        void bindMesh();

    };
}

#endif //VULKANCOMMANDBUFFER_H
