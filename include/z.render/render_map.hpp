#include <SDL3/SDL.h>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <random>

#include "main.h"
#include "debug.h"
#include "locations.hpp"
#include "info.h"
#include "player_info.h"
#include "helper_utils.h"
#include "texture_agent.hpp"

inline bool NEED_MAP_UPDATE = true;

bool render_combat_inital(std::string map_name);
bool render_combat(SDL_Renderer* renderer, int player_x, int player_y);

struct TileMetadata {
    std::vector<Hitbox> moveboxes;
    std::vector<Hitbox> hitboxes;
    
    std::string debug_out() {
        std::string metadata_str = "{}";
        return metadata_str;
    }
};

struct Tile {
    std::string id;
    std::string path;
    int size;
    int rotation = 0;
    TileMetadata metadata;

    std::string debug_out() const {
        return id + ", " + path + ", " + std::to_string(size) /*+ ", " + metadata.debug_out()*/; // TODO: fix metadata.debug_out()
    }
};

struct MapSettings {
    XY starting_pos = {0, 0};
};

class Map {
    private:
        TextureAgent* map_texture_agent;
        std::vector<std::vector<std::vector<Tile>>> map_tiles;
        int current_tile_x;
        int current_tile_y;
    public:
        // Vars //
        MapSettings settings;
        // Init and cleanup //
        Map(const std::string& map_name, SDL_Renderer* renderer);
        ~Map();

        // Helper functions //
        Tile load_tile(nlohmann::json tile);
        TileMetadata get_tile_metadata(nlohmann::json metadata_json);

        // Render Map //
        void render_map(int camera_x, int camera_y);
};