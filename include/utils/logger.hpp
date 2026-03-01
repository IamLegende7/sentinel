#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <SDL3/SDL.h>

// Log rotation
#include <cstdio>

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
            previous_time = std::chrono::steady_clock::now();
        }

        ~Logger() {
            if (log_file.is_open()) {
                log_file.close();
            }
        }

        void log(const char* file, const char* func, LogLevel level, const char* format, ...) {
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
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - previous_time).count();
            previous_time = now;

            // Convert elapsed time into seconds and milliseconds
            long seconds = elapsed_ms / 1000;
            long milliseconds = elapsed_ms % 1000;

            std::ostringstream timeStream;
            timeStream << seconds << '.' << std::setfill('0') << std::setw(3) << milliseconds;

            std::string timestamp = timeStream.str();
            std::string full_log        = prefix + ": [" + timestamp + "]" + "[" + std::string(base_name(file)) + ":" + std::string(func) + "] " + message;
            std::string full_log_colour = color_code + prefix + "\033[0m" + ": [" + timestamp + "]" + "[" + std::string(base_name(file)) + ":" + std::string(func) + "] " + message;

            if (log_file.is_open()) {
                log_file << full_log << std::endl;  
            }
            if (level == LogLevel::ERROR || level == LogLevel::CRITICAL) {
                SDL_LogError(SDL_LOG_PRIORITY_ERROR, "%s", full_log_colour.c_str());
            } else {
                SDL_Log("%s", full_log_colour.c_str());
            }
        }

        bool set_logfile(const std::string& filename) {
            remove(filename.c_str());
            log_file.open(filename, std::ios::out | std::ios::app);
            if (!log_file.is_open()) {
                SDL_Log("\033[31mERROR\033[0m: Could not open log file: %s", filename.c_str());
                return false;
            }
            log_file << "--- Cleared and Set log file to " << filename << " ---"<< std::endl;
            return true;
        }

    private:
        std::ofstream log_file;
        std::chrono::steady_clock::time_point start_time;
        std::chrono::steady_clock::time_point previous_time;
        const char* base_name(const char* path) {
            const char* p = strrchr(path, '/');
            if (!p) p = strrchr(path, '\\');
            return p ? p+1 : path;
        }
};

inline Logger LOGGER;
#define LOG(level, format, ...) LOGGER.log(__FILE__, __func__, level, format, ##__VA_ARGS__)

#endif