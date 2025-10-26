#include "render_map.hpp"
#include <iostream>
#include <random>

/* This class handels renderring and loading of the maps
   Use one instance for each map!
   ```map_name``` must be passed in WITH file extention
*/

//////////////////////
// INIT AND CLEANUP //
//////////////////////

Map::Map(const std::string& map_name, SDL_Renderer* renderer) {
    map_texture_agent = new TextureAgent(renderer);
    map_texture_agent->load_texture(PATH_MISSING_TEXTURE_TILE.c_str(), PATH_MISSING_TEXTURE_TILE);
    //// LOAD MAP ////
    LOGGER.log(LogLevel::DEBUG, "NOW LOADING THE MAP %s!", map_name.c_str());
    nlohmann::json json_map_data = get_json(MAP_DIR + "/" + map_name); // no worry about errors; handling in ```get_json```

    // SETTINGS //
    nlohmann::json json_settings = json_map_data["settings"];
    if (json_settings.contains("starting_pos")) {
        settings.starting_pos.x = json_settings["starting_pos"][0].get<int>();
        settings.starting_pos.y = json_settings["starting_pos"][1].get<int>();
    }
    LOGGER.log(LogLevel::DEBUG, "Starting X: %d, Y: %d", settings.starting_pos.x, settings.starting_pos.y);

    // TILES //
    nlohmann::json json_tile_data = json_map_data["tiles"];
    map_tiles.clear();

    // GET ALL TILES IN MAP //
    for (const auto& row : json_tile_data) {
        std::vector<std::vector<Tile>> new_row;

        // GET TILES IN ROW //
        for (const auto& tile_list : row) {
            std::vector<Tile> new_tile_list;
            if (tile_list.is_object()) {
                nlohmann::json tile = tile_list;
                new_tile_list.push_back(load_tile(tile));
            } else if (tile_list.is_array()) {
                for (nlohmann::json tile : tile_list) {
                    new_tile_list.push_back(load_tile(tile));
                }
            }

            // PUSH BACK //
            new_row.push_back(new_tile_list);
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

Tile Map::load_tile(nlohmann::json tile) {
    Tile new_tile;
    try {

        // GET ID //
        new_tile.id = tile["id"].get<std::string>();


        nlohmann::json json_tile_defaults = get_json(PATH_DEFAULTS_TILES)["data"][new_tile.id];

        // GET PATH //
        if (tile.contains("texture")) {
            // Use the given texture path //
            new_tile.path = tile["texture"].get<std::string>();
        } else {
            // Get tile path from tile_defaults.json //

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
            std::string tile_path = PATH_MISSING_TEXTURE_TILE;
            if (json_tile_defaults["texture"].is_string()) {
                std::string tile_texture = json_tile_defaults["texture"].get<std::string>();
                tile_path = tile_path_prefix + tile_texture;

            } else if (json_tile_defaults["texture"].is_array()) {
                std::vector<std::string> tile_path_end_vector = json_tile_defaults["texture"].get<std::vector<std::string>>();
                if (!tile_path_end_vector.empty()) {
                    std::random_device rd;
                    std::mt19937 gen(rd()); // Seed with a real random value, if available
                    std::uniform_int_distribution<> dis(0, tile_path_end_vector.size() - 1);
                    int random_index = dis(gen);
                    tile_path = tile_path_prefix + tile_path_end_vector[random_index];

                } else {
                    LOGGER.log(LogLevel::ERROR, "[render/render_map.cpp:Map] Could not load texture for tile: tile_paths empty");
                }
            }
            //LOGGER.log(LogLevel::DEBUG, "Tile path: %s", tile_path.c_str());

            // give out
            new_tile.path = tile_path;
        }

        // GET SIZE //
        if (tile.contains("size")) {
            new_tile.size = tile["size"].get<int>();
        } else if (json_tile_defaults.contains("size")) {
            new_tile.size = json_tile_defaults["size"].get<int>();
        } else { 
            new_tile.size = DEFAULT_SIZE_TILE;
        }

        // GET ROTATION //
        if (tile.contains("rotation")) {
            new_tile.rotation = 90 * tile["rotation"].get<int>();
        } else if (json_tile_defaults.contains("rotation")) {
            new_tile.rotation = 90 * json_tile_defaults["rotation"].get<int>();
        }

        // GET MATADATA //
        if (tile.contains("metadata")) {
            new_tile.metadata = get_tile_metadata(tile["metadata"]);
        } else if (json_tile_defaults.contains("metadata")) {
            new_tile.metadata = get_tile_metadata(json_tile_defaults["metadata"]);
        }

        // ADD TO TEXTURE AGENT //
        if (map_texture_agent->load_texture(new_tile.path.c_str(), new_tile.path) == 1) {
            LOGGER.log(LogLevel::ERROR, "[render/render_map.cpp:Map] Failed to load texture");
        }
    
    } catch (...) {
        LOGGER.log(LogLevel::ERROR, "[render/render_map.cpp:Map] Uncaught exception loading a Tile. Using 'missing' tile");
        new_tile.id     =     "sentinel:missing";
        new_tile.path   =     PATH_MISSING_TEXTURE_TILE;
        new_tile.size   =     DEFAULT_SIZE_TILE;
    }
    return new_tile;
}

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
            for (std::vector<std::vector<Tile>> row : map_tiles) {
                if (y_tile < min_y || y_tile > max_y) {
                    y_tile++;
                    continue;
                }
                int final_y = (y_tile * (DEFAULT_SIZE_TILE * ZOOM)) - camera_y;
                int x_tile = 0;
                for (std::vector<Tile> tile_list : row) {
                    if (x_tile < min_x || x_tile > max_x) {
                        x_tile++;
                        continue;
                    }
                    for (Tile& tile : tile_list) {
                        int final_x = (x_tile * (tile.size * ZOOM)) - camera_x;
                        map_texture_agent->render_texture(tile.path, final_x, final_y, tile.size * ZOOM, tile.rotation);
                    }
                    x_tile++;
                }
            y_tile++;
            }
        }