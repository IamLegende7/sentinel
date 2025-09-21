#include <SDL3/SDL.h>
#include <stdio.h>

#include "render_map.h"

Map main_map;

int render_map_inital(std::string map_name, SDL_Renderer* renderer) {
    main_map.pass_renderer(renderer);
    main_map.load_map(map_name);
    return 0;
}

int render_map_main(SDL_Renderer* renderer, int player_x, int player_y) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    main_map.render_map(player_x, player_y);
    SDL_RenderPresent(renderer);
    return 0;
}