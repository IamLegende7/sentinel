#ifndef SETTINGS_LOCATIONS_H
#define SETTINGS_LOCATIONS_H

#include "utils/config.hpp"

#include <regex>
#include <string>

/* This is where most file locations are stored for later use in the main scripts

*/

inline std::unordered_map<std::string, Setting> LOCATIONS;

inline std::string replace_locations(std::string input_string) {
    std::string output_string = input_string;
    for (const auto& pair : LOCATIONS) {
        output_string = std::regex_replace(output_string, std::regex("\\$" + pair.first + "\\$"), std::string(pair.second));    
    }

    return output_string;
}

inline void init_locations_settings(std::string config_file) {
    // Main dirs //
    LOCATIONS["data_dir"] =             Setting(replace_locations(load_setting<std::string>(config_file, "Main dirs", "data_dir")));
    LOCATIONS["config_dir"] =           Setting(replace_locations(load_setting<std::string>(config_file, "Main dirs", "config_dir")));
    LOCATIONS["resource_dir"] =         Setting(replace_locations(load_setting<std::string>(config_file, "Main dirs", "resource_dir")));
    // Maps //
    LOCATIONS["map_dir"] =              Setting(replace_locations(load_setting<std::string>(config_file, "Maps", "map_dir")));
    // Textures //
    LOCATIONS["texture_dir"] =          Setting(replace_locations(load_setting<std::string>(config_file, "Textures", "texture_dir")));
    LOCATIONS["texturepack_dir"] =      Setting(replace_locations(load_setting<std::string>(config_file, "Textures", "texturepack_dir")));
    LOCATIONS["missing_texture_tile"] = Setting(replace_locations(load_setting<std::string>(config_file, "Textures", "missing_texture_tile")));
    LOCATIONS["textures_json"] =        Setting(replace_locations(load_setting<std::string>(config_file, "Textures", "textures_json")));
    // Logging //
    LOCATIONS["log_file"] =             Setting(replace_locations(load_setting<std::string>(config_file, "Logging", "log_file")));
    LOCATIONS["log_crash_dir"] =        Setting(replace_locations(load_setting<std::string>(config_file, "Logging", "log_crash_dir")));
    // Registry //
    LOCATIONS["units_json"] =           Setting(replace_locations(load_setting<std::string>(config_file, "Registry", "units_json")));
    LOCATIONS["tiles_json"] =           Setting(replace_locations(load_setting<std::string>(config_file, "Registry", "tiles_json")));
}

#endif