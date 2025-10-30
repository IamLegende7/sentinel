#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <SDL3/SDL.h>

// Time
#include <chrono>
#include <iomanip>

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
        Logger() {
            start_time = std::chrono::steady_clock::now();
        }

        ~Logger() {
            if (log_file.is_open()) {
                log_file.close();
            }
        }

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
            std::string color_code;
            switch (level) {
                    case LogLevel::DEBUG:
                        prefix = "DEBUG   ";
                        color_code = "\033[32m"; // Green
                        break;
                    case LogLevel::INFO:     
                        prefix = "INFO    ";
                        color_code = "\033[37m"; // White
                        break;
                    case LogLevel::WARNING:
                        prefix = "WARNING ";
                        color_code = "\033[33m"; // Yellow
                        break;
                    case LogLevel::ERROR:
                        prefix = "ERROR   ";
                        color_code = "\033[31m"; // Red
                        break;
                    case LogLevel::CRITICAL:
                        prefix = "CRITICAL";
                        color_code = "\033[41;37m"; // Red background with white text
                        break;
            }

            // Get elapsed time
            auto now = std::chrono::steady_clock::now();
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();

            // Convert elapsed time into seconds and milliseconds
            long seconds = elapsed_ms / 1000;
            long milliseconds = elapsed_ms % 1000;

            std::ostringstream timeStream;
            timeStream << seconds << '.' << std::setfill('0') << std::setw(3) << milliseconds;

            std::string timestamp = timeStream.str();
            std::string full_log        = prefix + ": [" + timestamp + "] " + message;
            std::string full_log_colour = color_code + prefix + "\033[0m" + ": [" + timestamp + "] " + message;

            if (log_file.is_open()) {
                log_file << full_log << std::endl;
            }
            SDL_Log("%s", full_log_colour.c_str());
        }

        bool set_logfile(const std::string& filename) {
            log_file.open(filename, std::ios::out | std::ios::app);
            if (!log_file.is_open()) {
                SDL_Log("\033[31mERROR\033[0m: Could not open log file: %s", filename.c_str());
                return false;
            }
            return true;
        }

    private:
        std::ofstream log_file;
        std::chrono::steady_clock::time_point start_time;
};

inline Logger LOGGER;

#endif