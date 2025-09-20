#ifndef LOCATIONS_H
#define LOCATIONS_H

#include <string>

const std::string RESOURCE_DIR = "resources";
inline const std::string MAP_DIR = RESOURCE_DIR + "/maps";
inline const std::string TEXTURE_DIR = RESOURCE_DIR + "/textures";
inline std::string PATH_DEFAULT_TEXTURE_TILES_JSON = "/defaults/texture_tile_defaults.json";
inline std::string PATH_MISSING_TEXTURE_TILE = "/tiles/missing.png";

#endif