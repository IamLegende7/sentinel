#include "map/map.hpp"

/* This class handels renderring and loading of the maps
   Use one instance for each map!
   ```map_name``` must be passed in WITH file extention

   Loading map logic was moved to render/load_map.cpp
*/

//////////////////////
// INIT AND CLEANUP //
//////////////////////

Map::~Map() {
    delete map_texture_agent;
    map_texture_agent = nullptr;
}

////////////////
// RENDER MAP //
////////////////

void Map::render_map(int camera_x, int camera_y) {
            int min_x = (camera_x / 100);
            int max_x = ((camera_x + (SCREEN_WIDTH / ZOOM))  / 100);
            int min_y = (camera_y / 100);
            int max_y = ((camera_y + (SCREEN_HEIGHT / ZOOM)) / 100);

            if (DEBUG["show_tile_hiding"]) {
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