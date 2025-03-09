//
// Created by lepag on 3/8/2025.
//

#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
class VulkanContext;

namespace Trin::Runtime::Core {
class Engine {
public:
    Engine();
    bool init();
    bool run();
    bool shutdown();
private:

};

}

#endif //ENGINE_H
