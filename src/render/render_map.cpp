#include "render_map.hpp"
#include <iostream>

#include "thread_pool.hpp"

/* This class handels renderring and loading of the maps
   Use one instance for each map!
   ```map_name``` must be passed in WITH file extention
*/

//////////////////////
// INIT AND CLEANUP //
//////////////////////

Map::Map(const std::string& map_name, SDL_Renderer* renderer) {
    map_texture_agent = new TextureAgent(renderer);
    map_texture_agent->load_texture(LOCATIONS["missing_texture_tile"].get().c_str(), LOCATIONS["missing_texture_tile"].get());
    //// LOAD MAP ////
    LOGGER.log(LogLevel::INFO, "Using Texturepack: %s", get_json(LOCATIONS["textures_json"].get())["name"].get<std::string>().c_str());
    LOGGER.log(LogLevel::INFO, "NOW LOADING THE MAP %s!", map_name.c_str());
    nlohmann::json json_map_data = get_json(LOCATIONS["map_dir"].get() + "/" + map_name); // no worry about errors; handling in ```get_json```
    LOGGER.log(LogLevel::INFO, "Map size: %dx%d", json_map_data["size"][0].get<int>(), json_map_data["size"][1].get<int>());
    MOVEBOXES = new HitboxQuadtree({0, 0}, {json_map_data["size"][0].get<int>() * 100, json_map_data["size"][1].get<int>() * 100});

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
    XY current_tile = {0, 0};
    if (SETTINGS_BOOL["multithreading"].get()) {
        ThreadPool* map_pool = new ThreadPool(SETTINGS_INT["multithreading"].get());
        std::vector<std::future<std::vector<std::vector<Tile>>> > futures;

        // Get data //
        for (const auto& row : json_tile_data) {
            current_tile.x = 0;
            auto future = map_pool->enqueue([this, row, current_tile]() {
                return get_row(row, current_tile);
            });

            futures.push_back(std::move(future));

            current_tile.y++;
        }

        // Extract data //
        for (auto& future : futures) {
            std::vector<std::vector<Tile>> new_row = future.get();

            map_tiles.push_back(new_row);

            for (auto new_tile_list : new_row) {
                for (Tile new_tile : new_tile_list) {
                    // ADD TO TEXTURE AGENT //
                    if (map_texture_agent->load_texture(new_tile.path.c_str(), new_tile.path) == 1) {
                        LOGGER.log(LogLevel::ERROR, "[render/render_map.cpp:Map] Failed to load texture");
                    }
                    // ADD TO QUADTREE //
                    for (Hitbox new_movebox : new_tile.metadata.moveboxes)
                        MOVEBOXES->insert(new_movebox);
                }
            }
        }

        delete map_pool;

    } else {
        current_tile = {0, 0};
        for (const auto& row : json_tile_data) {
            std::vector<std::vector<Tile>> new_row;

            current_tile.x = 0;
            new_row = get_row(row, current_tile);

            for (auto new_tile_list : new_row) {
                for (Tile new_tile : new_tile_list) {
                    // ADD TO TEXTURE AGENT //
                    if (map_texture_agent->load_texture(new_tile.path.c_str(), new_tile.path) == 1) {
                        LOGGER.log(LogLevel::ERROR, "[render/render_map.cpp:Map] Failed to load texture");
                    }
                    for (Hitbox new_movebox : new_tile.metadata.moveboxes)
                        MOVEBOXES->insert(new_movebox);
                }
            }
            map_tiles.push_back(new_row);

            current_tile.y++;
        }
    }

    LOGGER.log(LogLevel::DEBUG, "DONE LOADING THE MAP");
}

Map::~Map() {
    delete map_texture_agent;
    map_texture_agent = nullptr;
}

//////////////////////
// HELPER FUNCTIONS //
//////////////////////

std::vector<std::vector<Tile>> Map::get_row(nlohmann::json row, XY current_tile) {
    std::vector<std::vector<Tile>> new_row;

    // GET TILES IN ROW //
    for (const auto& tile_list : row) {
        std::vector<Tile> new_tile_list;
        if (tile_list.is_object()) {
            nlohmann::json tile = tile_list;
            Tile new_tile = load_tile(tile, current_tile);
            if (!new_tile.unit) new_tile_list.push_back(new_tile);
        } else if (tile_list.is_array()) {
            for (nlohmann::json tile : tile_list) {
                Tile new_tile = load_tile(tile, current_tile);
                if (!new_tile.unit) new_tile_list.push_back(new_tile);
            }
        }

        // PUSH BACK //
        new_row.push_back(new_tile_list);

        current_tile.x++;
    }

    return new_row;
}

Tile Map::load_tile(nlohmann::json tile, XY current_tile) {
    Tile new_tile;
    try {
        // GET ID //
        new_tile.id = tile["id"].get<std::string>();

        if (tile.contains("unit")) {
            LOGGER.log(LogLevel::WARNING, "No unit spawn logic yet!");
            new_tile.unit = true;
        } else {
            nlohmann::json json_tile_defaults = nlohmann::json{};
            if (get_json(LOCATIONS["default_tiles_json"].get()).contains(new_tile.id)) {
                json_tile_defaults = get_json(LOCATIONS["default_tiles_json"].get())[new_tile.id];
            } else {
                LOGGER.log(LogLevel::WARNING, "%s doesn't contain %s. Might use 'missing' tile", LOCATIONS["default_tiles_json"].get().c_str(), new_tile.id.c_str());
            }

            nlohmann::json json_textures = nlohmann::json{};
            if (get_json(LOCATIONS["textures_json"].get())["data"].contains(new_tile.id)) {
                json_textures = get_json(LOCATIONS["textures_json"].get())["data"][new_tile.id];
            } else {
                LOGGER.log(LogLevel::WARNING, "%s doesn't contain %s. Might use 'missing' tile", LOCATIONS["textures_json"].get().c_str(), new_tile.id.c_str());
            }

            // GET PATH //
            std::string tile_path = LOCATIONS["missing_texture_tile"].get();
            if (tile.contains("texture")) {
                // Use the given texture path //
                tile_path = tile.get<std::string>();
            } else {
                // Get tile path from tile_defaults.json //
                if (json_textures.is_string()) {
                    tile_path = json_textures.get<std::string>();

                } else if (json_textures.is_array()) {
                    std::vector<std::string> tile_path_end_vector = json_textures.get<std::vector<std::string>>();
                    if (!tile_path_end_vector.empty()) {
                        std::random_device rd;
                        std::mt19937 gen(rd()); // Seed with a real random value, if available
                        std::uniform_int_distribution<> dis(0, tile_path_end_vector.size() - 1);
                        int random_index = dis(gen);
                        tile_path = tile_path_end_vector[random_index];

                    } else {
                        LOGGER.log(LogLevel::ERROR, "[render/render_map.cpp:Map] Could not load texture for tile: tile_paths empty");
                    }
                }
                if (DEBUG["all_debug_logs"].get()) {
                    LOGGER.log(LogLevel::DEBUG, "Tile path: %s", tile_path.c_str());
                }
            }
            // replace substrings //
            tile_path = replace_locations(tile_path);

            // give out //
            new_tile.path = tile_path;

            // GET ROTATION //
            if (tile.contains("rotation")) {
                new_tile.rotation = 90 * tile["rotation"].get<int>();
            } else if (json_tile_defaults.contains("rotation")) {
                new_tile.rotation = 90 * json_tile_defaults["rotation"].get<int>();
            }

            // GET METADATA //
            if (tile.contains("metadata")) {
                new_tile.metadata = get_tile_metadata(tile["metadata"]);
            } else if (json_tile_defaults.contains("metadata")) {
                new_tile.metadata = get_tile_metadata(json_tile_defaults["metadata"]);
            }

            // ADD HITBOXES //
            if ((tile.contains("metadata") and tile["metadata"].contains("movebox")) or (json_tile_defaults.contains("metadata") and json_tile_defaults["metadata"].contains("movebox"))) {
                for (auto& movebox : new_tile.metadata.moveboxes) {
                    // rotate //
                    switch (new_tile.rotation) {
                        case 90: {
                            movebox.y_offset = 100 - (movebox.y_offset + movebox.height);
                            XY offset =  {movebox.x_offset, movebox.y_offset};
                            movebox.x_offset = offset.y;
                            movebox.y_offset = offset.x;
                            XY dimensions = {movebox.width, movebox.height};
                            movebox.width  = dimensions.y;
                            movebox.height = dimensions.x;
                            break;
                        }
                        case 180: {
                            movebox.x_offset = 100 - (movebox.x_offset + movebox.width);
                            movebox.y_offset = 100 - (movebox.y_offset + movebox.height);
                            break;
                        }
                        case 270: {
                            movebox.x_offset = 100 - (movebox.x_offset + movebox.width);
                            XY offset =  {movebox.x_offset, movebox.y_offset};
                            movebox.x_offset = offset.y;
                            movebox.y_offset = offset.x;
                            XY dimensions = {movebox.width, movebox.height};
                            movebox.width  = dimensions.y;
                            movebox.height = dimensions.x;
                            break;
                        }
                    }
                    // offset //
                    movebox.x = current_tile.x * 100;
                    movebox.y = current_tile.y * 100;
                    movebox.x_offset += new_tile.metadata.size;
                    movebox.y_offset += new_tile.metadata.size;

                    if (DEBUG["all_debug_logs"].get()) {
                        LOGGER.log(LogLevel::DEBUG, "New Hitbox: Type: %d | X,Y: %d %d | X_off,Y_off: %d %d | Width,Height: %d %d", movebox.type, movebox.x, movebox.y, movebox.x_offset, movebox.y_offset, movebox.width, movebox.height);
                    }
                }
            }
        }
    } catch (...) {
        LOGGER.log(LogLevel::ERROR, "[render/render_map.cpp:Map] Uncaught exception loading a Tile. Using 'missing' tile");
    }
    return new_tile;
}

TileMetadata Map::get_tile_metadata(nlohmann::json metadata_json) {
    TileMetadata metadata;
    // GET SIZE //
    if (metadata_json.contains("size")) {
        metadata.size = metadata_json["size"].get<int>();
    }

    if (metadata_json.contains("movebox")) {
        for (nlohmann::json movebox_json_single : metadata_json["movebox"]) {
            Hitbox new_movebox = {
                movebox_json_single[0].get<int>(),
                0, 0,
                movebox_json_single[3].get<int>(),
                movebox_json_single[4].get<int>(),
                movebox_json_single[5].get<int>(),
                movebox_json_single[6].get<int>()
            };
            metadata.moveboxes.push_back(new_movebox);
        }
    }
    if (metadata_json.contains("hitbox")) {
        for (nlohmann::json hitbox_json_single : metadata_json["hitbox"]) {
            Hitbox new_hitbox = {
                hitbox_json_single[0].get<int>(),
                0, 0,
                hitbox_json_single[1].get<int>(),
                hitbox_json_single[2].get<int>(),
                hitbox_json_single[3].get<int>(),
                hitbox_json_single[4].get<int>()
            };
            metadata.hitboxes.push_back(new_hitbox);
            if (DEBUG["all_debug_logs"].get()) {
                LOGGER.log(LogLevel::DEBUG, "New Hitbox: Type: %d | X_off,Y_off: %d %d | Width,Height: %d %d", new_hitbox.type, new_hitbox.x_offset, new_hitbox.y_offset, new_hitbox.width, new_hitbox.height);
            }
        }
    }
    return metadata;
}

////////////////
// RENDER MAP //
////////////////

void Map::render_map(int camera_x, int camera_y) {
            int min_x = (camera_x / 100);
            int max_x = ((camera_x + (SCREEN_WIDTH / ZOOM))  / 100);
            int min_y = (camera_y / 100);
            int max_y = ((camera_y + (SCREEN_HEIGHT / ZOOM)) / 100);

            if (DEBUG["show_tile_hiding"].get()) {
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
                float final_y = ((y_tile * 100) - camera_y) * ZOOM;
                int x_tile = 0;
                for (std::vector<Tile> tile_list : row) {
                    if (x_tile < min_x || x_tile > max_x) {
                        x_tile++;
                        continue;
                    }
                    for (Tile& tile : tile_list) {
                        float final_x = ((x_tile * 100) - camera_x) * ZOOM;
                        map_texture_agent->render_texture(tile.path, final_x, final_y, tile.metadata.size * ZOOM, tile.rotation);
                    }
                    x_tile++;
                }
            y_tile++;
            }
        }