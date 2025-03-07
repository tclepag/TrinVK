//
// Created by lepag on 3/6/2025.
//

#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <mutex>
#include <ostream>
#include <string>

namespace Trin::Helpers {
    enum class LogLevel {
        Info,
        Warning,
        Error,
        Debug,
        Markdown
    };
class Console {
public:
    /**
     * @brief Prints a basic string to the console
     * @param message The message to output to the console
     */
    static void print(const std::string& message) {
        logMessage(LogLevel::Info, message);
    }

    /**
     * @brief Prints a string tagged with WARNING to the console
     * @param message The message to output to the console
     */
    static void warn(const std::string& message) {
        logMessage(LogLevel::Warning, message);
    }

    /**
     * @brief Prints a string tagged with ERROR to the console
     * @param message The message to output to the console
     */
    static void error(const std::string& message) {
        logMessage(LogLevel::Error, message);
    }

    /**
     * @brief Prints a string tagged with DEBUG to the console
     * @param message The message to output to the console
     */
    static void debug(const std::string& message) {
        logMessage(LogLevel::Debug, message);
    }

    static void markdown(const std::string& message) {
        logMessage(LogLevel::Markdown, message);
    }

private:
    Console() = default;
    ~Console() = default;

    static std::mutex sm_console_mutex;

    /**
     * @brief Internal behavior for the public static print functions
     * @param level The log context to use, info, debug, error, etc..
     * @param message The string to output to the console
     */
    static void logMessage(const LogLevel level, const std::string& message) { // TODO create editor console, to print well structured messages
        std::lock_guard lock(sm_console_mutex);
        switch (level) {
            case LogLevel::Info: {
                std::cout << message << std::endl;
                break;
            }
            case LogLevel::Warning: {
                std::cout << "[WARNING]:" << message << std::endl;
                break;
            }
            case LogLevel::Error: {
                std::cerr << "[ERROR]:" << message << std::endl;
                break;
            }
            case LogLevel::Debug: {
                std::cout << "[DEBUG]: " << message << std::endl;
                break;
            }
            case LogLevel::Markdown: { // TODO add markdown support after creating editor console
                std::cout << message << std::endl;
                break;
            }
        }
    }
};

}


#endif //CONSOLE_H
