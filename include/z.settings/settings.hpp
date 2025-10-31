#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "config.hpp"

inline std::unordered_map<std::string, Setting<bool>> SETTINGS_BOOL;
inline std::unordered_map<std::string, Setting<int>> SETTINGS_INT;

inline void init_main_settings(std::string config_file) {
    // Mulithreading //
    SETTINGS_BOOL["multithreading"] =             Setting<bool>("multithreading", config_file);
    SETTINGS_INT["multithreading"]  =             Setting<int>("num_threads",     config_file);
}

#endif