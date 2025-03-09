//
// Created by lepag on 3/8/2025.
//

#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <windows.h>

namespace Trin::Helpers {
    class System {
      public:
        static void SetHWND(HWND hWnd) {
              hwnd = hWnd;
        }
        static void PostFatalError(const std::string& title, const std::string& message) {
            MessageBox(hwnd, reinterpret_cast<LPCWSTR>(message.c_str()), reinterpret_cast<LPCWSTR>(title.c_str()), MB_ICONERROR | MB_OK);
        }
        private:
          static HWND hwnd;
    };
}

#endif //SYSTEM_H
