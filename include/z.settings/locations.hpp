#ifndef LOCATIONS_H
#define LOCATIONS_H

#include "config.hpp"

#include "helper_utils.h"

/* This is where most file locations are stored for later use in the main scripts

*/

inline std::unordered_map<std::string, Setting<std::string>> LOCATIONS;

inline std::string replace_locations(std::string input_string) {
    std::string output_string;
    output_string = replace_substring(input_string,  "$data_dir$",        LOCATIONS["data_dir"].get());
    output_string = replace_substring(output_string, "$data_dir$",        LOCATIONS["data_dir"].get());
    output_string = replace_substring(output_string, "$config_dir$",      LOCATIONS["config_dir"].get());
    output_string = replace_substring(output_string, "$resource_dir$",    LOCATIONS["resource_dir"].get());

    output_string = replace_substring(output_string, "$map_dir$",         LOCATIONS["map_dir"].get());

    output_string = replace_substring(output_string, "$texture_dir$",     LOCATIONS["texture_dir"].get());
    output_string = replace_substring(output_string, "$texturepack_dir$", LOCATIONS["texturepack_dir"].get());
        
    output_string = replace_substring(output_string, "$log_crash_dir$",   LOCATIONS["log_crash_dir"].get());

    return output_string;
}

inline void init_locations_settings(std::string config_file) {
    // Main dirs //
    LOCATIONS["data_dir"] =             Setting<std::string>("data_dir",                config_file);
    LOCATIONS["config_dir"] =           Setting<std::string>("config_dir",              config_file);
    LOCATIONS["resource_dir"] =         Setting<std::string>("resource_dir",            config_file);
    // Maps //
    LOCATIONS["map_dir"] =              Setting<std::string>("map_dir",                 config_file);
    // Textures //
    LOCATIONS["texture_dir"] =          Setting<std::string>("texture_dir",             config_file);
    LOCATIONS["texturepack_dir"] =      Setting<std::string>("texturepack_dir",         config_file);
    LOCATIONS["missing_texture_tile"] = Setting<std::string>("missing_texture_tile",    config_file);
    LOCATIONS["textures_json"] =        Setting<std::string>("textures_json",           config_file);
    // Logging //
    LOCATIONS["log_file"] =             Setting<std::string>("log_file",                config_file);
    LOCATIONS["log_crash_dir"] =        Setting<std::string>("log_crash_dir",           config_file);
    // Registry //
    LOCATIONS["units_json"] =          Setting<std::string>("units_json",               config_file);
    LOCATIONS["tiles_json"] =          Setting<std::string>("tiles_json",               config_file);

    for (auto it = LOCATIONS.begin(); it != LOCATIONS.end(); ++it) {
        it->second.set(replace_locations(it->second.get()));
    }

}


#endif