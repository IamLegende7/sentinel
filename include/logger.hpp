#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <SDL3/SDL.h>

/////////////
// LOGGING //
/////////////

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger {
    public:
        void log(LogLevel level, const char* format, ...) {
            std::ostringstream oss;
                va_list args;
                va_start(args, format);
                
                char buffer[256];
                vsnprintf(buffer, sizeof(buffer), format, args);
                va_end(args);
            oss << buffer;

            std::string message = oss.str();

            if (!message.empty() && message.back() == '\n') {
                message.pop_back();
            }

            std::string prefix;
            std::string colorCode;
            switch (level) {
                    case LogLevel::DEBUG:
                        prefix = "DEBUG";
                        colorCode = "\033[32m"; // Green
                        break;
                    case LogLevel::INFO:     
                        prefix = "INFO";
                        colorCode = "\033[37m"; // White
                        break;
                    case LogLevel::WARNING:
                        prefix = "WARNING";
                        colorCode = "\033[33m"; // Yellow
                        break;
                    case LogLevel::ERROR:
                        prefix = "ERROR";
                        colorCode = "\033[31m"; // Red
                        break;
                    case LogLevel::CRITICAL:
                        prefix = "CRITICAL";
                        colorCode = "\033[41;37m"; // Red background with white text
                        break;
            }
            if (log_file.is_open()) {
                log_file << prefix << ": " << message << std::endl;
            }
            std::string fullLog = colorCode + prefix + "\033[0m" + ": " + message;
            SDL_Log("%s", fullLog.c_str());
        }

        bool set_log_file(const std::string& filename) {
            log_file.open(filename, std::ios::out | std::ios::app);
            if (!log_file.is_open()) {
                SDL_Log("\033[31mERROR\033[0m: Could not open log file: %s", filename.c_str());
                return false;
            }
            return true;
        }

        ~Logger() {
            if (log_file.is_open()) {
                log_file.close();
            }
        }

    private:
        std::ofstream log_file;
};

// The actual Logger instance //
inline Logger LOGGER;

#endif