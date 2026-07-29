#include "map/map.hpp"

#include "utils/thread_pool.hpp"

/* Some functions were moved here because they are too long.
   They handle the loading logic of the map

*/

//////////
// INIT //
//////////

Map::Map(const std::string& map_name, SDL_Renderer* renderer) {
    map_texture_agent = new TextureAgent(renderer);
    map_texture_agent->load_texture(std::string(LOCATIONS["missing_texture_tile"]).c_str(), std::string(LOCATIONS["missing_texture_tile"]));
    //// LOAD MAP ////
    // Logging
    const char* texturepack_name = 
        open_json(std::string(LOCATIONS["textures_json"])).HasMember("name") ?
            open_json(std::string(LOCATIONS["textures_json"]))["name"].GetString() :
            "None/NoName";
    LOGGER.log(LogLevel::INFO, "Using Texturepack: %s", texturepack_name);
    LOGGER.log(LogLevel::INFO, "NOW LOADING THE MAP %s!", map_name.c_str());

    // Loading json
    const rapidjson::Document json_map_data = open_json(std::string(LOCATIONS["map_dir"]) + "/" + map_name); // don't worry about errors; handling in ```get_json```

    // Logging size
    XY map_size = {0, 0};
    if (json_map_data.HasMember("size") && json_map_data["size"].IsArray()) {
        LOGGER.log(LogLevel::DEBUG, "Hewwo!");
        map_size.x = json_map_data["size"][0].GetFloat();
        map_size.y = json_map_data["size"][1].GetFloat();
    }
    LOGGER.log(LogLevel::INFO, "Map size: %dx%d", map_size.x, map_size.y);

    // Making Quadtree for Hitboxes
    MOVEBOXES = new HitboxQuadtree({0, 0}, {map_size.x * 100, map_size.y * 100});

    // SETTINGS //
    if (json_map_data.HasMember("settings")) {
        const rapidjson::Value& json_settings = json_map_data["settings"];
        if (json_settings.HasMember("starting_pos")) {
            settings.starting_pos.x = json_settings["starting_pos"][0].GetInt();
            settings.starting_pos.y = json_settings["starting_pos"][1].GetInt();
        }
        LOGGER.log(LogLevel::DEBUG, "Starting X: %d, Y: %d", (int)(settings.starting_pos.x), (int)(settings.starting_pos.y));
    }

    // TILES //
    if (!json_map_data.HasMember("tiles")) {
        LOGGER.log(LogLevel::ERROR, "No 'tiles' object specified in map '%s'", map_name.c_str());
    } else {
        const rapidjson::Value& json_tile_data = json_map_data["tiles"];
        map_tiles.clear();

        // GET ALL TILES IN MAP //
        XY current_tile = {0, 0};
        if (SETTINGS["multithreading"]) {
            ThreadPool* map_pool = new ThreadPool(SETTINGS["num_threads"]);
            std::vector<std::future<std::vector<std::vector<Tile>>>> futures;

            // Temp store data //
            for (const rapidjson::Value& row : json_tile_data.GetArray()) {
                current_tile.x = 0;
                auto future = map_pool->enqueue([this, &row, &current_tile]() {
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
                    }
                }
            }

            delete map_pool;

        } else {
            current_tile = {0, 0};
            for (const auto& row : json_tile_data.GetArray()) {
                std::vector<std::vector<Tile>> new_row;

                current_tile.x = 0;
                new_row = get_row(row, current_tile);

                for (auto new_tile_list : new_row) {
                    for (Tile new_tile : new_tile_list) {
                        // ADD TO TEXTURE AGENT //
                        if (map_texture_agent->load_texture(new_tile.path.c_str(), new_tile.path) == 1) {
                            LOGGER.log(LogLevel::ERROR, "[render/render_map.cpp:Map] Failed to load texture");
                        }
                    }
                }
                map_tiles.push_back(new_row);

                current_tile.y++;
            }
        } // if !multithreading
    } // if map.HasMember("tiles")

    LOGGER.log(LogLevel::DEBUG, "DONE LOADING THE MAP");
} // Function "Map::Map"

//////////////////////
// HELPER FUNCTIONS //
//////////////////////

std::vector<std::vector<Tile>> Map::get_row(const rapidjson::Value& row, XY current_tile) {
    std::vector<std::vector<Tile>> result_row;

    // GET TILES IN ROW //
    for (const rapidjson::Value& tile_list : row.GetArray()) {
        std::vector<Tile> new_tile_list;
        if (tile_list.IsObject()) {
            const rapidjson::Value& tile = tile_list;
            Tile new_tile = load_tile(tile, current_tile);
            if (!new_tile.unit) new_tile_list.push_back(new_tile);
        } else if (tile_list.IsArray()) {
            for (const rapidjson::Value& tile : tile_list.GetArray()) {
                Tile new_tile = load_tile(tile, current_tile);
                if (!new_tile.unit) new_tile_list.push_back(new_tile);
            }
        }

        // PUSH BACK //
        result_row.push_back(new_tile_list);

        current_tile.x++;
    }

    return result_row;
}

Tile Map::load_tile(const rapidjson::Value& tile, const XY& current_tile) {
    Tile new_tile;
    try {
        // GET ID //
        new_tile.id = "sentinel:missing";
        if (tile.HasMember("id")) {
            new_tile.id = tile["id"].GetString();
        } else {
            LOGGER.log(LogLevel::WARNING, "No ID found for tile at (%s | %s)", current_tile.x, current_tile.y);
        }

        LOGGER.log(LogLevel::DEBUG, "ID loaded!"); /* REMOVE LATER! */

        if (tile.HasMember("unit")) {
            LOGGER.log(LogLevel::WARNING, "No unit spawn logic yet!");
            new_tile.unit = true;
        } else {
            rapidjson::Value json_tile_defaults = rapidjson::Value(rapidjson::kObjectType);
            if (open_json(LOCATIONS["tiles_json"]).HasMember(new_tile.id.c_str())) {
                json_tile_defaults = open_json(LOCATIONS["tiles_json"])[new_tile.id.c_str()];
            } else {
                LOGGER.log(LogLevel::WARNING, "%s doesn't contain %s. Might use 'missing' tile", std::string(LOCATIONS["tiles_json"]).c_str(), new_tile.id.c_str());
            }

            LOGGER.log(LogLevel::DEBUG, "tile_defaults loaded!"); /* REMOVE LATER! */

            rapidjson::Value json_textures = rapidjson::Value(rapidjson::kObjectType);
            if (open_json(LOCATIONS["textures_json"]).HasMember("data")) {
                if (open_json(LOCATIONS["textures_json"])["data"].HasMember(new_tile.id.c_str())) {
                    json_textures = open_json(LOCATIONS["textures_json"])["data"][new_tile.id.c_str()];
                } else {
                    LOGGER.log(LogLevel::WARNING, "%s doesn't contain %s. Might use 'missing' tile", std::string(LOCATIONS["textures_json"]).c_str(), new_tile.id.c_str());
                }
            } else {
                LOGGER.log(LogLevel::WARNING, "[map/load_map.cpp:load_tile] %s does not have a 'data' member", std::string(LOCATIONS["textures_json"]).c_str());
            }

            LOGGER.log(LogLevel::DEBUG, "texture_defaults loaded!"); /* REMOVE LATER! */

            // GET PATH //
            std::string texture_path = LOCATIONS["missing_texture_tile"];
            if (tile.HasMember("texture")) {
                // Use the given texture path //
                texture_path = tile.GetString();
            } else {
                // Get tile path from tile_defaults.json //
                if (json_textures.IsObject()) {
                    texture_path = json_textures.GetString();

                } else if (json_textures.IsArray()) {
                    std::vector<std::string> texture_path_vector;
                    for (const auto& element : json_textures.GetArray()) {
                        if (element.IsString()) {
                            texture_path_vector.push_back(element.GetString());
                        }
                    }
                    if (!texture_path_vector.empty()) {
                        std::random_device rd;
                        std::mt19937 gen(rd());
                        std::uniform_int_distribution<> dis(0, texture_path_vector.size() - 1);
                        int random_index = dis(gen);
                        texture_path = texture_path_vector[random_index];

                    } else {
                        LOGGER.log(LogLevel::ERROR, "[render/render_map.cpp:Map] Could not load texture for tile: tile_paths empty");
                    }
                }
                if (DEBUG["all_debug_logs"]) {
                    LOGGER.log(LogLevel::DEBUG, "Tile path: %s", texture_path.c_str());
                }
            }
            // replace substrings //
            texture_path = replace_locations(texture_path);

            LOGGER.log(LogLevel::DEBUG, "Path loaded!"); /* REMOVE LATER! */

            // give out //
            new_tile.path = texture_path;

            // GET ROTATION //
            if (tile.HasMember("rotation")) {
                new_tile.rotation = 90 * tile["rotation"].GetInt();
            } else if (json_tile_defaults.HasMember("rotation")) {
                new_tile.rotation = 90 * json_tile_defaults["rotation"].GetInt();
            }

            // GET METADATA //
            if (tile.HasMember("metadata")) {
                new_tile.metadata = get_tile_metadata(tile["metadata"]);
            } else if (json_tile_defaults.HasMember("metadata")) {
                new_tile.metadata = get_tile_metadata(json_tile_defaults["metadata"]);
            }

            // ADD HITBOXES //
            if (tile["metadata"].HasMember("movebox")) {
                for (const rapidjson::Value& hitbox_json : tile["metadata"]["movebox"].GetArray()) {
                    MOVEBOXES->insert(get_hitbox(hitbox_json, new_tile, current_tile));
                }
            }
            /* if (tile["metadata"].HasMember("hitbox")) {
                for (const rapidjson::Value& hitbox_json : tile["metadata"]["hitbox"].GetArray()) {
                    HITBOXES->insert(get_hitbox(hitbox_json, new_tile, current_tile));
                }
            } */
        }

    } catch (...) {
        LOGGER.log(LogLevel::ERROR, "[render/render_map.cpp:Map] Uncaught exception loading a Tile. Using 'missing' tile");
    }
    return new_tile;
}

TileMetadata Map::get_tile_metadata(const rapidjson::Value& metadata_json) {
    TileMetadata metadata;
    // GET SIZE //
    if (metadata_json.HasMember("size")) {
        metadata.size = metadata_json["size"].GetInt();
    }
    return metadata;
}

Hitbox Map::get_hitbox(const rapidjson::Value& hitbox_json, const Tile& new_tile, const XY& current_tile) {
    // LOAD //
    Hitbox new_hitbox = {
        hitbox_json[0].GetInt(),
        0, 0,
        hitbox_json[3].GetFloat(),
        hitbox_json[4].GetFloat(),
        hitbox_json[5].GetFloat(),
        hitbox_json[6].GetFloat()
    };

    // ROTATE //
    switch (new_tile.rotation) {
        case 90: {
            new_hitbox.y_offset = 100 - (new_hitbox.y_offset + new_hitbox.height);
            XY offset =  {new_hitbox.x_offset, new_hitbox.y_offset};
            new_hitbox.x_offset = offset.y;
            new_hitbox.y_offset = offset.x;
            XY dimensions = {new_hitbox.width, new_hitbox.height};
            new_hitbox.width  = dimensions.y;
            new_hitbox.height = dimensions.x;
            break;
        }
        case 180: {
            new_hitbox.x_offset = 100 - (new_hitbox.x_offset + new_hitbox.width);
            new_hitbox.y_offset = 100 - (new_hitbox.y_offset + new_hitbox.height);
            break;
        }
        case 270: {
            new_hitbox.x_offset = 100 - (new_hitbox.x_offset + new_hitbox.width);
            XY offset =  {new_hitbox.x_offset, new_hitbox.y_offset};
            new_hitbox.x_offset = offset.y;
            new_hitbox.y_offset = offset.x;
            XY dimensions = {new_hitbox.width, new_hitbox.height};
            new_hitbox.width  = dimensions.y;
            new_hitbox.height = dimensions.x;
            break;
        }

        // OFFSET //
        new_hitbox.x = current_tile.x * 100;
        new_hitbox.y = current_tile.y * 100;
        new_hitbox.x_offset += new_tile.metadata.size;
        new_hitbox.y_offset += new_tile.metadata.size;

        // DEBUG //
        if (DEBUG["all_debug_logs"]) {
            LOGGER.log(LogLevel::DEBUG, "New Hitbox: Type: %d | X,Y: %d %d | X_off,Y_off: %d %d | Width,Height: %d %d", new_hitbox.type, new_hitbox.x, new_hitbox.y, new_hitbox.x_offset, new_hitbox.y_offset, new_hitbox.width, new_hitbox.height);
        }
    };

    return new_hitbox;
}