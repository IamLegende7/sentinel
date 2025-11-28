#ifndef DEBUG_H
#define DEBUG_H

#include "utils/config.hpp"

/* Detailed debug mode settings

*/

inline std::unordered_map<std::string, Setting> DEBUG;

inline void init_debug_settings(std::string config_file) {
    // Misc //
    DEBUG["can_reload_map"] =           Setting(load_setting<bool>(config_file, "Misc", "can_reload_map", false));
    // Renderring //
    DEBUG["show_tile_hiding"] =         Setting(load_setting<bool>(config_file, "Renderring", "show_tile_hiding", false));
    // Hitboxes //
    DEBUG["show_hitboxes"] =            Setting(load_setting<bool>(config_file, "Hitboxes", "show_hitboxes", false));
    DEBUG["show_relevant_hitboxes"] =   Setting(load_setting<bool>(config_file, "Hitboxes", "show_relevant_hitboxes", false));
    DEBUG["show_nodes"] =               Setting(load_setting<bool>(config_file, "Hitboxes", "show_nodes", false));
    // Movement //
    DEBUG["show_coords"] =              Setting(load_setting<bool>(config_file, "Movement", "show_coords", false));
    DEBUG["show_speed"] =               Setting(load_setting<bool>(config_file, "Movement", "show_speed", false));
    // Logging // 
    DEBUG["test_logger"] =              Setting(load_setting<bool>(config_file, "Logging", "test_logger", false));
    DEBUG["all_debug_logs"] =           Setting(load_setting<bool>(config_file, "Logging", "all_debug_logs", false));
    DEBUG["annoying_logs"] =            Setting(load_setting<bool>(config_file, "Logging", "annoying_logs", false));
};

#endif