#ifndef RENDER_MAP_H
#define RENDER_MAP_H

#include <string>

inline int DEFAULT_SIZE_TILE = 100;
inline bool NEED_MAP_UPDATE = true;


bool render_map_inital(std::string map_name);
bool render_main(SDL_Renderer* renderer, int player_x, int player_y);

struct TileMetadata {

};

struct Tile {
    std::string id;
    std::string texture;
    int size;
    TileMetadata metadata;
};

#endif