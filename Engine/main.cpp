#include <windows.h>

#include "Core/Engine.h"
#include "Helpers/Console.h"

using namespace Trin::Runtime::Core;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
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
