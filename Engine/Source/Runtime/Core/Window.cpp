//
// Created by lepag on 3/8/2025.
//

#include "Window.h"


namespace Trin::Runtime::Core {

    bool Window::init() {
        // Create Window Class
        WNDCLASSEX winClass = {};
        winClass.lpszClassName = L"TRINVK";
        winClass.hInstance = GetModuleHandle(nullptr);
        winClass.lpfnWndProc = WndProc;
        // Register the class
        RegisterClassEx(&winClass);

        // Create Window
        m_hWnd = CreateWindowEx(
            0,
            winClass.lpszClassName,
            L"TRINVK",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
            nullptr,
            nullptr,
            winClass.hInstance,
            nullptr
        );

        if (m_hWnd == nullptr) {
            System::PostFatalError("WINDOW FAILURE", "FAILED TO CREATE WIN32 WINDOW");
            return false;
        }

        System::SetHWND(m_hWnd);
        ShowWindow(m_hWnd, SW_SHOW);
        UpdateWindow(m_hWnd);
        return true;
    }

    LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}
