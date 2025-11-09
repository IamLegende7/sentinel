#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "config.hpp"

inline std::unordered_map<std::string, Setting<bool>> SETTINGS_BOOL;
inline std::unordered_map<std::string, Setting<int>> SETTINGS_INT;
inline std::unordered_map<std::string, Setting<std::string>> SETTINGS_STRING;

inline void init_main_settings(std::string config_file) {
    // Mulithreading //
    SETTINGS_BOOL["multithreading"] =             Setting<bool>("multithreading",       config_file);
    SETTINGS_INT["multithreading"]  =             Setting<int>("num_threads",           config_file);
    SETTINGS_BOOL["old_renderring"] =             Setting<bool>("old_renderring",       config_file);
    SETTINGS_STRING["gpu_driver"]   =             Setting<std::string>("gpu_driver",    config_file);
}

#endif