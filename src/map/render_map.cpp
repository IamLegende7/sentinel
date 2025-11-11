#include "map/render_map.hpp"

#include "utils/thread_pool.hpp"

/* This class handels renderring and loading of the maps
   Use one instance for each map!
   ```map_name``` must be passed in WITH file extention

   Loading map logic was moved to render/load_map.cpp
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
    MOVEBOXES = new HitboxQuadtree({0, 0}, {json_map_data["size"][0].get<float>() * 100, json_map_data["size"][1].get<float>() * 100});

    // SETTINGS //
    nlohmann::json json_settings = json_map_data["settings"];
    if (json_settings.contains("starting_pos")) {
        settings.starting_pos.x = json_settings["starting_pos"][0].get<int>();
        settings.starting_pos.y = json_settings["starting_pos"][1].get<int>();
    }
    LOGGER.log(LogLevel::DEBUG, "Starting X: %d, Y: %d", (int)(settings.starting_pos.x), (int)(settings.starting_pos.y));

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