//
// Created by lepag on 3/8/2025.
//

#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>

namespace Trin::Helpers {
    class System {
      public:
        static void PostFatalError(const std::string& title, const std::string& message) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), message.c_str(), nullptr);
        }
    };
}

#endif //SYSTEM_H
