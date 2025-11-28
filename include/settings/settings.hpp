#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "utils/config.hpp"

inline std::unordered_map<std::string, Setting> SETTINGS;

inline void init_main_settings(std::string config_file) {
    // Mulithreading //
    SETTINGS["multithreading"] =   Setting(load_setting<bool>(config_file, "Mulithreading", "multithreading", false));
    printf("Hello\n");
    SETTINGS["num_threads"] =      Setting(load_setting<int>(config_file, "Mulithreading", "num_threads", 1));
    printf("Bye\n");
    // Renderring
    SETTINGS["old_renderring"] =   Setting(load_setting<bool>(config_file, "Renderring", "old_renderring", true));
    SETTINGS["gpu_driver"] =       Setting(load_setting<std::string>(config_file, "Renderring", "gpu_driver"));
}

#endif