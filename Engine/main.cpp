#include <iostream>
#include "Core/Engine.h"

using namespace Trin::Runtime::Core;

int main() {
    try {
        const auto engine = std::make_unique<Engine>();
        if (bool result = engine->init(); !result) {
            std::cerr << "Failed to initialize engine!" << std::endl;
            return -1;
        }
        engine->run();
        engine->shutdown();

    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}
