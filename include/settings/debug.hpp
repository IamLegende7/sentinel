#ifndef DEBUG_H
#define DEBUG_H

#include "utils/config.hpp"

/* Detailed debug mode settings

*/

inline std::unordered_map<std::string, Setting<bool>> DEBUG;

inline void init_debug_settings(std::string config_file) {
    // Misc //
    DEBUG["can_reload_map"] =          Setting<bool>("can_reload_map",           config_file);
    // Renderring //
    DEBUG["show_tile_hiding"] =        Setting<bool>("show_tile_hiding",         config_file);
    // Hitboxes //
    DEBUG["show_hitboxes"] =           Setting<bool>("show_hitboxes",            config_file);
    DEBUG["show_relevant_hitboxes"] =  Setting<bool>("show_relevant_hitboxes",   config_file);
    DEBUG["show_nodes"] =              Setting<bool>("show_nodes",               config_file);
    // Movement //
    DEBUG["show_coords"] =             Setting<bool>("show_coords",              config_file);
    DEBUG["show_speed"] =              Setting<bool>("show_speed",               config_file);
    // Logging // 
    DEBUG["test_logger"] =             Setting<bool>("test_logger",              config_file);
    DEBUG["all_debug_logs"] =          Setting<bool>("all_debug_logs",           config_file);
    DEBUG["annoying_logs"] =           Setting<bool>("annoying_logs",            config_file);
}

#endif