#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <iostream>
#include <sstream>
#include <tuple>
#include <utility>

#define FMT_HEADER_ONLY
#include "fmt/core.h"
#include "fmt/color.h"

using namespace std; 
  
// enum to represent log levels 
enum LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL }; 
  
class Logger { 
public: 
    LogLevel log_level = INFO;
    string logger_name;

    Logger(string name, const string &level = "INFO") {
        set_level(level);
        logger_name = name;
    }

    ~Logger() {} 

    // log a debug message
    template <class... Args>
    void debug(fmt::format_string<Args...> message, Args... args) { 
        log_with_args(DEBUG, message, args...);
    }

    void debug(const string &message) { 
        log(DEBUG, message);
    }

    // log an info message
    template <class... Args>
    void info(fmt::format_string<Args...> message, Args... args) { 
        log_with_args(INFO, message, args...);
    }

    void info(const string &message) { 
        log(INFO, message);
    }

    // log a warning message
    template <class... Args>
    void warning(fmt::format_string<Args...> message, Args... args) { 
        log_with_args(WARNING, message, args...);
    }

    void warning(const string &message) { 
        log(WARNING, message);
    }

    // log a error message
    template <class... Args>
    void error(fmt::format_string<Args...> message, Args... args) { 
        log_with_args(ERROR, message, args...);
    }

    void error(const string &message) { 
        log(ERROR, message);
    }

    // log a critical message
    template <class... Args>
    void critical(fmt::format_string<Args...> message, Args... args) { 
        log_with_args(CRITICAL, message, args...);
    }

    void critical(const string &message) { 
        log(CRITICAL, message);
    }

    // set the log level
    void set_level(LogLevel level) { 
        log_level = level; 
    }

    // set the log level from a string
    void set_level(const string &level) { 
        set_level(string_to_level(level)); 
    }
  

private: 

    // logs a message with a given log level 
    template <class... Args>
    void log_with_args(LogLevel level, fmt::format_string<Args...> message, Args... args) { 

        // check if loglevel is reached
        if (level < log_level) {
            return;
        }
        
        // combine string with arguments
        string formatted_message = format(message, forward<Args>(args)...);

        // log it
        log(level, formatted_message);
    }

    void log(LogLevel level, const string &message) { 

        // check if loglevel is reached
        if (level < log_level) {
            return;
        }

        if (level >= WARNING) {
            // bold red for warnings and errors
            fmt::print(fg(fmt::color::red), "[{}] [{}] {}\n", level_to_string(level), logger_name, message);
        } else {
            // normal output to console 
            fmt::print(fg(fmt::color::gray), "[{}] [{}] {}\n", level_to_string(level), logger_name, message);
        }
    } 
  
    // converts log level to a string
    string level_to_string(LogLevel level) 
    { 
        switch (level) { 
        case DEBUG: 
            return "DEBUG"; 
        case INFO: 
            return "INFO"; 
        case WARNING: 
            return "WARNING"; 
        case ERROR: 
            return "ERROR"; 
        case CRITICAL: 
            return "CRITICAL"; 
        default: 
            return "UNKNOWN"; 
        } 
    } 

    // converts a string to log level
    LogLevel string_to_level(const string& level) { 
        if (level == "DEBUG") 
            return DEBUG; 
        if (level == "INFO") 
            return INFO; 
        if (level == "WARNING") 
            return WARNING; 
        if (level == "ERROR") 
            return ERROR; 
        if (level == "CRITICAL") 
            return CRITICAL; 
        return INFO; 
    }
}; 

#endif // LOGGING_HPP