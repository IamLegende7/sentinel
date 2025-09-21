#ifndef RENDER_MAP_H
#define RENDER_MAP_H

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

#include "main.h"
#include "player_info.h"
#include "locations.h"
#include "info.h"
#include "render_data.h"
#include "helper_utils.h"


inline int DEFAULT_SIZE_TILE = 100;


struct TileMetadata {

};

struct Tile {
    std::string id;
    std::string texture;
    int size;
    TileMetadata metadata;
};

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
            int y_tile = 0;
            for (std::vector<Tile> vector_1d : map_data) {
                int final_y = (y_tile * (DEFAULT_SIZE_TILE * ZOOM)) - player_y;
                int x_tile = 0;
                for (Tile& tile_data : vector_1d) {
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
        
    private:
        TextureManager map_textures_manager;
        Tile invalid = {INFO_NAMESPACE + "INVALID", "background/not_found.png", DEFAULT_SIZE_TILE, {}};
        std::vector<std::vector<Tile>> map_data;
};

#endif