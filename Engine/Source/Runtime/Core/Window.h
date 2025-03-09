//
// Created by lepag on 3/8/2025.
//

#ifndef WINDOW_H
#define WINDOW_H
#include <memory>

#include <windows.h>
#include <vulkan/vulkan_core.h>

#include "Helpers/System.h"


namespace Trin::Runtime::Core {
    using namespace Helpers;
class Window {
public:
    bool init();
    [[nodiscard]] bool shouldClose() const { return m_shouldClose; }
    void setToClose(const bool close) { m_shouldClose = close; }
    [[nodiscard]] HWND getHandle() const { return m_hWnd; }
private:
    bool m_shouldClose = false;
    HWND m_hWnd;


    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

}

#endif //WINDOW_H
