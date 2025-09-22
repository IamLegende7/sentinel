#include <SDL3/SDL.h>
#include <stdio.h>

#include "render_map.h"

Map main_map;

int render_map_inital(std::string map_name, SDL_Renderer* renderer) {
    main_map.pass_renderer(renderer);
    main_map.load_map(map_name);
    return 0;
}

int render_map_main(int player_x, int player_y) {
    SDL_SetRenderDrawColor(MAIN_REN, 0, 0, 0, 255);
    SDL_RenderClear(MAIN_REN);
    main_map.render_map(player_x, player_y);
    return 0;
}