#ifndef LOCATIONS_H
#define LOCATIONS_H

#include <string>

/* This is where most file locations are stored for later use in the main scripts

*/

// MAIN //
const std::string DATA_DIR     =                    "data";
const std::string RESOURCE_DIR =                    "resources";
const std::string TEXTURE_DIR =                     RESOURCE_DIR + "/textures";
const std::string TEXTUREPACK_DIR =                 RESOURCE_DIR + "/texturepacks";

// MAPS //
const std::string MAP_DIR =                         RESOURCE_DIR + "/maps";

// TEXTURES //
const std::string PATH_DEFAULT_TEXTURE_TILES =      TEXTURE_DIR  + "/defaults/texture_tile_defaults.jsonc";
const std::string PATH_MISSING_TEXTURE_TILE =       TEXTURE_DIR  + "/tiles/missing.png";
const std::string PATH_DEFAULTS_TILES =             RESOURCE_DIR + "/registry/default.jsonc";
//const std::string PATH_DEFAULTS_TILES =             RESOURCE_DIR + "/texturepacks/programerart/programer.jsonc"; // TODO: add a way to switch between texturepacks in-game

// LOGGING //
const std::string PATH_LOG_FILE =                   DATA_DIR     + "/main.log";

#endif