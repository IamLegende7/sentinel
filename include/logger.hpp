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

class FileLogger {
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
            if (logFile.is_open()) {
                logFile << prefix << ": " << message << std::endl;
            }
            std::string fullLog = colorCode + prefix + "\033[0m" + ": " + message;
            SDL_Log("%s", fullLog.c_str());
        }

        bool set_logfile(const std::string& filename) {
            logFile.open(filename, std::ios::out | std::ios::app);
            if (!logFile.is_open()) {
                SDL_Log("\033[31mERROR\033[0m: Could not open log file: %s", filename.c_str());
                return false;
            }
            return true;
        }

        ~FileLogger() {
            if (logFile.is_open()) {
                logFile.close();
            }
        }

    private:
        std::ofstream logFile;
};

// The actual Logger instance //
inline FileLogger LOGGER;

#endif