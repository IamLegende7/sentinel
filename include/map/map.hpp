#ifndef RENDER_MAP_HPP
#define RENDER_MAP_HPP

#include <SDL3/SDL.h>
#include <vector>
#include <string>
#include <random>

#include "main.h"
#include "player_info.h"
#include "settings/settings.hpp"
#include "settings/debug.hpp"
#include "settings/locations.hpp"
#include "utils/json.hpp"
#include "utils/hitboxes.hpp"
#include "render/texture_agent.hpp"
#include "loops/combat_vars.hpp"

inline bool NEED_MAP_UPDATE = true;

bool render_combat_inital(std::string map_name);
bool render_combat(SDL_Renderer* renderer, int player_x, int player_y);

struct TileMetadata {
    int size = DEFAULT_SIZE_TILE;
};

struct Tile {
    std::string id = "sentinel:missing";
    bool unit = false;
    std::string path = LOCATIONS["missing_texture_tile"];
    int rotation = 0;
    TileMetadata metadata;
};

struct MapSettings {
    XY starting_pos = XY{0, 0};
};

class Map {
    private:
        TextureAgent* map_texture_agent;
        std::vector<std::vector<std::vector<Tile>>> map_tiles;
    public:
        // Vars //
        MapSettings settings;
        // Init and cleanup //
        Map(const std::string& map_name, SDL_Renderer* renderer);
        ~Map();

        // Helper functions //
        std::vector<std::vector<Tile>> get_row(const rapidjson::Value& row, XY current_tile);
        Tile load_tile(const rapidjson::Value& tile, const XY& current_tile);

        TileMetadata get_tile_metadata(const rapidjson::Value& metadata_json);
        Hitbox get_hitbox(const rapidjson::Value& hitbox_json, const Tile& new_tile, const XY& current_tile);

        // Render Map //
        void render_map(int camera_x, int camera_y);
};

#endif