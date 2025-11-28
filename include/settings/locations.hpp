#ifndef LOCATIONS_H
#define LOCATIONS_H

#include "utils/config.hpp"

#include "utils/helper_utils.h"

/* This is where most file locations are stored for later use in the main scripts

*/

inline std::unordered_map<std::string, Setting> LOCATIONS;

inline std::string replace_locations(std::string input_string) {
    std::string output_string;
    output_string = replace_substring(input_string,  "$data_dir$",        LOCATIONS["data_dir"]);
    output_string = replace_substring(output_string, "$data_dir$",        LOCATIONS["data_dir"]);
    output_string = replace_substring(output_string, "$config_dir$",      LOCATIONS["config_dir"]);
    output_string = replace_substring(output_string, "$resource_dir$",    LOCATIONS["resource_dir"]);

    output_string = replace_substring(output_string, "$map_dir$",         LOCATIONS["map_dir"]);

    output_string = replace_substring(output_string, "$texture_dir$",     LOCATIONS["texture_dir"]);
    output_string = replace_substring(output_string, "$texturepack_dir$", LOCATIONS["texturepack_dir"]);
        
    output_string = replace_substring(output_string, "$log_crash_dir$",   LOCATIONS["log_crash_dir"]);

    return output_string;
}

inline void init_locations_settings(std::string config_file) {
    // Main dirs //
    LOCATIONS["data_dir"] =             Setting(load_setting<std::string>(config_file, "Main dirs", "data_dir"));
    LOCATIONS["config_dir"] =           Setting(load_setting<std::string>(config_file, "Main dirs", "config_dir"));
    LOCATIONS["resource_dir"] =         Setting(load_setting<std::string>(config_file, "Main dirs", "resource_dir"));
    // Maps //
    LOCATIONS["map_dir"] =              Setting(load_setting<std::string>(config_file, "Maps", "map_dir"));
    // Textures //
    LOCATIONS["texture_dir"] =          Setting(load_setting<std::string>(config_file, "Textures", "texture_dir"));
    LOCATIONS["texturepack_dir"] =      Setting(load_setting<std::string>(config_file, "Textures", "texturepack_dir"));
    LOCATIONS["missing_texture_tile"] = Setting(load_setting<std::string>(config_file, "Textures", "missing_texture_tile"));
    LOCATIONS["textures_json"] =        Setting(load_setting<std::string>(config_file, "Textures", "textures_json"));
    // Logging //
    LOCATIONS["log_file"] =             Setting(load_setting<std::string>(config_file, "Logging", "log_file"));
    LOCATIONS["log_crash_dir"] =        Setting(load_setting<std::string>(config_file, "Logging", "log_crash_dir"));
    // Registry //
    LOCATIONS["units_json"] =           Setting(load_setting<std::string>(config_file, "Registry", "units_json"));
    LOCATIONS["tiles_json"] =           Setting(load_setting<std::string>(config_file, "Registry", "tiles_json"));

    for (auto it = LOCATIONS.begin(); it != LOCATIONS.end(); ++it) {
        it->second.set(replace_locations(it->second));
    }

}


#endif