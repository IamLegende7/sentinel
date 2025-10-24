#include "render_map.hpp"
#include <iostream>

/* This class handels renderring of the maps
   Use one instance for each map
   ```map_name``` must be passed in with file extention
*/

//////////////////////
// INIT AND CLEANUP //
//////////////////////

Map::Map(const std::string& map_name, SDL_Renderer* renderer) {
    map_texture_agent = new TextureAgent(renderer);
    map_texture_agent->load_texture(PATH_MISSING_TEXTURE_TILE.c_str(), "missing");
    // LOAD MAP //
    LOGGER.log(LogLevel::DEBUG, "NOW LOADING THE MAP %s!", map_name.c_str());
    // TODO: remove + ".jsonc" vvv
    nlohmann::json json_map_data = get_json(MAP_DIR + "/" + map_name + ".jsonc"); // no worry about errors; handling in ```get_json```
    nlohmann::json json_settings = json_map_data["settings"];
    nlohmann::json json_tile_data = json_map_data["tiles"];
    map_tiles.clear();

    // GET ALL TILES IN MAP //
    for (const auto& row : json_tile_data) {
        std::vector<Tile> new_row;

        // GET TILES IN ROW //
        for (const auto& tile : row) {
            Tile new_tile;

            // GET ID //
            new_tile.id = tile["id"].get<std::string>();

            // GET PATH //
            if (tile.contains("texture")) {
                // Use the given texture path //
                new_tile.path = tile["texture"].get<std::string>();
            } else {
                // Get tile path from texture_tile_defaults.json //

                // first partial
                std::string tile_path_prefix = "";
                std::string tile_namespace = split(new_tile.id, ':')[0];
                nlohmann::json json_namespaces = get_json(PATH_NAMESPACES);
                if (json_namespaces.contains(tile_namespace)) {
                    tile_path_prefix = json_namespaces[tile_namespace].get<std::string>();
                    tile_path_prefix = replace_substring(tile_path_prefix, "$textures$", TEXTURE_DIR);
                    tile_path_prefix = replace_substring(tile_path_prefix, "$resources$", RESOURCE_DIR);
                } else {
                    LOGGER.log(LogLevel::WARNING, "[render/render_map.cpp:Map] Failed to load tile: Could not find namespace '%s' in '%s'", tile_namespace, PATH_NAMESPACES);
                }

                // last partial
                nlohmann::json json_tile_defaults = get_json(PATH_DEFAULT_TEXTURE_TILES);
                std::string tile_path_end = json_tile_defaults[new_tile.id].get<std::string>();

                // full
                std::string tile_path = tile_path_prefix + tile_path_end;
                new_tile.path = tile_path;
            }

            // GET SIZE //
            if (tile.contains("size")) {
                new_tile.size = tile["size"].get<int>();
            } else { 
                new_tile.size = DEFAULT_SIZE_TILE;
            }

            // GET MATADATA //
            if (tile.contains("metadata")) {
                new_tile.metadata = get_tile_metadata(tile["metadata"]);
            }

            // ADD TO TEXTURE AGENT //
            if (map_texture_agent->load_texture(new_tile.path.c_str(), new_tile.id) == 1) {
                LOGGER.log(LogLevel::ERROR, "[render/render_map.cpp:Map] Failed to load texture");
            }

            // PUSH BACK //
            new_row.push_back(new_tile);
        }

        // PUSH BACK ROW //
        map_tiles.push_back(new_row);
    }

    LOGGER.log(LogLevel::DEBUG, "DONE LOADING THE MAP");
}

Map::~Map() {
    delete map_texture_agent;
}

//////////////////////
// HELPER FUNCTIONS //
//////////////////////

TileMetadata Map::get_tile_metadata(nlohmann::json metadata_json) {
    TileMetadata metadata;
    return metadata;
}

////////////////
// RENDER MAP //
////////////////

void Map::render_map(int camera_x, int camera_y) {
            int min_x = (camera_x / (DEFAULT_SIZE_TILE * ZOOM));
            int max_x = (camera_x + SCREEN_WIDTH) / (DEFAULT_SIZE_TILE * ZOOM);
            int min_y = (camera_y / (DEFAULT_SIZE_TILE * ZOOM));
            int max_y = (camera_y + SCREEN_HEIGHT) / (DEFAULT_SIZE_TILE * ZOOM);

            if (DEBUG_SHOW_TILE_HIDING) {
                min_x += 1;
                max_x -= 1;
                min_y += 1;
                max_y -= 1;
            }

            int y_tile = 0;
            for (std::vector<Tile> vector_1d : map_tiles) {
                if (y_tile < min_y || y_tile > max_y) {
                    y_tile++;
                    continue;
                }
                int final_y = (y_tile * (DEFAULT_SIZE_TILE * ZOOM)) - camera_y;
                int x_tile = 0;
                for (Tile& tile_data : vector_1d) {
                    if (x_tile < min_x || x_tile > max_x) {
                        x_tile++;
                        continue;
                    }
                    int final_x = (x_tile * (DEFAULT_SIZE_TILE * ZOOM)) - camera_x;
                    map_texture_agent->render_texture(tile_data.id, final_x, final_y, DEFAULT_SIZE_TILE * ZOOM);
                    x_tile++;
                }
            y_tile++;
            }
        }