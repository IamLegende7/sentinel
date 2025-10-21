#ifndef LOCATIONS_H
#define LOCATIONS_H

#include <string>

/* This is where most file locations are stored for later use in the main scripts

*/

const std::string RESOURCE_DIR = "resources";
const std::string MAP_DIR = RESOURCE_DIR + "/maps";
const std::string TEXTURE_DIR = RESOURCE_DIR + "/textures";
const std::string PATH_DEFAULT_TEXTURE_TILES_JSON = "/defaults/texture_tile_defaults.jsonc";
const std::string PATH_MISSING_TEXTURE_TILE = "/tiles/missing.png";

const std::string PATH_LOG_FILE = "data/main.log";

#endif