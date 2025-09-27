#include <SDL3/SDL.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include "render_map.h"
#include "main.h"
#include "player_info.h"
#include "locations.h"
#include "info.h"
#include "render_data.h"
#include "helper_utils.h"
#include "debug.h"

class Map {
    public:
        // Passes the renderer to ```TextureManager.set_renderer()```. Is required once before doing anything else with the ```TexturManager```
        void pass_renderer(SDL_Renderer* renderer) {
            map_textures_manager.set_renderer(renderer);
        }

        // Gets the default texture for a tile from the tile ´´´id´´´
        std::string get_default_texture(std::string id) {
            std::string id_sterile = replaceSubstring(id, INFO_NAMESPACE, "$vanilla$");
            std::string default_textures_path_full = TEXTURE_DIR + PATH_DEFAULT_TEXTURE_TILES_JSON;
            std::ifstream file(default_textures_path_full);
            if (!file.is_open()) {
                std::cerr << "Failed to open json file: " << default_textures_path_full << std::endl;
                return PATH_MISSING_TEXTURE_TILE;
            }
            nlohmann::json json_data;
            file >> json_data;
            if ( json_data.contains(id_sterile) ) {
                std::string texture_path = json_data[id_sterile].get<std::string>();
                file.close();
                return texture_path;
            }
            return PATH_MISSING_TEXTURE_TILE;
        }

        // Loads a map. Example usage: ```map.load_map(campain/debug)```
        // DO NOT add the .json file extention
        void load_map(const std::string& mapname) {
            std::string filename = mapname + ".json";
            std::ifstream file(MAP_DIR + "/" + filename);
            if (!file.is_open()) {
                std::cerr << "Failed to open map file: " << filename << std::endl;
                return;
            }

            // load the map data form the map.json
            nlohmann::json json_data;
            file >> json_data;
            nlohmann::json json_map_data = json_data["tiles"];
            map_data.clear();
            for (const auto& row : json_map_data) {
                std::vector<Tile> row_tiles;
                for (const auto& tile : row) {
                    Tile new_tile;
                    new_tile.id = replaceSubstring(tile["id"].get<std::string>(), "$vanilla$", INFO_NAMESPACE);
                    if ( tile.contains("texture") ) {
                        new_tile.texture = tile["texture"].get<std::string>();
                    } else {
                        new_tile.texture = get_default_texture(new_tile.id);
                    }
                    new_tile.size = tile.contains("size") ? tile["size"].get<int>() : DEFAULT_SIZE_TILE;
                    // Assuming metadata is an object; adjust as necessary
                    new_tile.metadata = TileMetadata();

                    row_tiles.push_back(new_tile);
                }
                map_data.push_back(row_tiles);
            }
            file.close();

            for (std::vector<Tile> vector_1d : map_data) {
                for (Tile& tile_data : vector_1d) {
                    map_textures_manager.load_texture(tile_data.texture.c_str(), tile_data.id);
                }
            }
        }

        void render_map(int player_x, int player_y) {
            int min_x = (player_x / (DEFAULT_SIZE_TILE * ZOOM));
            int max_x = (player_x + SCREEN_WIDTH) / (DEFAULT_SIZE_TILE * ZOOM);
            int min_y = (player_y / (DEFAULT_SIZE_TILE * ZOOM));
            int max_y = (player_y + SCREEN_HEIGHT) / (DEFAULT_SIZE_TILE * ZOOM);

            if (DEBUG_SHOW_TILE_HIDING) {
                min_x += 1;
                max_x -= 1;
                min_y += 1;
                max_y -= 1;
            }

            int y_tile = 0;
            for (std::vector<Tile> vector_1d : map_data) {
                if (y_tile < min_y || y_tile > max_y) {
                    y_tile++;
                    continue;
                }
                int final_y = (y_tile * (DEFAULT_SIZE_TILE * ZOOM)) - player_y;
                int x_tile = 0;
                for (Tile& tile_data : vector_1d) {
                    if (x_tile < min_x || x_tile > max_x) {
                        x_tile++;
                        continue;
                    }
                    int final_x = (x_tile * (DEFAULT_SIZE_TILE * ZOOM)) - player_x;
                    map_textures_manager.render_texture(tile_data.id, final_x, final_y, DEFAULT_SIZE_TILE);
                    x_tile++;
                }
            y_tile++;
            }
        }

        Tile get_tile(int x, int y) const {
            std::size_t x_cast = static_cast<std::size_t>(x);
            std::size_t y_cast = static_cast<std::size_t>(y);
            if (y_cast < map_data.size() && x_cast < map_data[y_cast].size()) {
                return map_data[y_cast][x_cast];
            }
            return invalid;
        }


        int getWidth() const {
            return map_data[0].size();
        }

        int getHeight() const {
            return map_data.size();
        }
        
        ~Map() {
            // Clean up
        }
    private:
        TextureManager map_textures_manager;
        Tile invalid = {INFO_NAMESPACE + "INVALID", "background/not_found.png", DEFAULT_SIZE_TILE, {}};
        std::vector<std::vector<Tile>> map_data;

};

/*
    ### VARIBLES ###
*/
Map main_map;                                       // the main map class handeling tiles
SDL_Texture* background_texture;                    // the background (the main map) as a single texture
SDL_Renderer* background_ren = nullptr;

bool make_background_texture(SDL_Renderer* renderer, int player_x, int player_y) {
    SDL_DestroyTexture(background_texture);
    background_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderTarget(renderer, background_texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);   // Black background
    SDL_RenderClear(renderer);
    main_map.render_map(player_x, player_y);
    SDL_SetRenderTarget(renderer, nullptr);
    return true;
}

bool render_combat_inital(std::string map_name) {
    background_ren = SDL_CreateRenderer(MAIN_WIN, NULL);
    main_map.pass_renderer(MAIN_REN);
    main_map.load_map(map_name);
    return true;
}

bool render_combat(SDL_Renderer* renderer, int player_x, int player_y) {

    // background (a.k.a the map)
    SDL_FRect full_window_rect = { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
    if (NEED_MAP_UPDATE) {
        make_background_texture(MAIN_REN, player_x - (SCREEN_WIDTH / 2) + 50, player_y - (SCREEN_HEIGHT / 2) + 50);
        NEED_MAP_UPDATE = false;
    }
    SDL_RenderTexture(renderer, background_texture, NULL, &full_window_rect);
    
    if (DEBUG) {  // debug info
        std::string picture_path_full = TEXTURE_DIR + "/crosshair_debug.png";
        SDL_Texture* crosshair_texture = IMG_LoadTexture(renderer, picture_path_full.c_str());
        SDL_SetTextureScaleMode(crosshair_texture, SDL_SCALEMODE_NEAREST);
        SDL_FRect crosshair_rect = { (float)(ceil(SCREEN_WIDTH / 2) - (DEFAULT_SIZE_TILE * ZOOM / 3)), (float)((SCREEN_HEIGHT / 2) - (DEFAULT_SIZE_TILE * ZOOM / 3)), (float)(DEFAULT_SIZE_TILE * ZOOM / 2), (float)(DEFAULT_SIZE_TILE * ZOOM / 2) };
        SDL_RenderTexture(MAIN_REN, crosshair_texture, NULL, &crosshair_rect);

            // render debug information   // TODO: change to actual text, not SDL debug text
        std::ostringstream coords;
        coords << "X: " << PLAYER.x << " Y: " << PLAYER.y;
        std::ostringstream speed;
        speed << "SpeedY: " << PLAYER.speed_y << " SpeedX: " << PLAYER.speed_x;
        SDL_SetRenderDrawColor(MAIN_REN, 255, 255, 255, 255);  // set coulor to white
        SDL_RenderDebugText(MAIN_REN, 5, 5, coords.str().c_str());
        SDL_RenderDebugText(MAIN_REN, 5, 20, speed.str().c_str());
    }
    return true;
}