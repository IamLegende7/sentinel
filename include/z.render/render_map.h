#ifndef RENDER_MAP_H
#define RENDER_MAP_H

#include <string>

inline bool NEED_MAP_UPDATE = true;


bool render_combat_inital(std::string map_name);
bool render_combat(SDL_Renderer* renderer, int player_x, int player_y);

struct TileMetadata {

};

struct Tile {
    std::string id;
    std::string texture;
    int size;
    TileMetadata metadata;
};

#endif