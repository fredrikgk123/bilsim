#pragma once

#include <iostream>
#include <string_view>

/**
 * Simple logging utility for the application
 * Provides consistent formatting for different log levels
 */
namespace Logger {
    enum class Level {
        INFO,
        WARNING,
        ERROR
    };

    inline void log(Level level, std::string_view message) {
        switch (level) {
            case Level::INFO:
                std::cout << "[INFO] " << message << std::endl;
                break;
            case Level::WARNING:
                std::cout << "[WARNING] " << message << std::endl;
                break;
            case Level::ERROR:
                std::cerr << "[ERROR] " << message << std::endl;
                break;
        }
    }

    inline void info(std::string_view message) {
        log(Level::INFO, message);
    }

    inline void warning(std::string_view message) {
        log(Level::WARNING, message);
    }

    inline void error(std::string_view message) {
        log(Level::ERROR, message);
    }
}

