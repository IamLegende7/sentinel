#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

#include "main.h"
#include "input.h"
#include "enemy.h"
#include "player_info.h"

int main(int argc, char *argv[]) {
    if ( !setup() ) {
        render_map_inital("campain/debug", MAIN_REN);
        PLAYER = init_player_unit(0, 0);
        render_map_main(MAIN_REN, PLAYER.x, PLAYER.y);

        // main game loop
        bool quit;
        SDL_Event e;
        while (!quit) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_EVENT_QUIT) quit = true;
                if (e.type == SDL_EVENT_WINDOW_RESIZED) {
                    SCREEN_WIDTH = e.window.data1;
                    SCREEN_HEIGHT = e.window.data2;
                    SDL_SetRenderViewport(MAIN_REN, NULL);
                    render_map_main(MAIN_REN, PLAYER.x, PLAYER.y);
                }
                if (movement_player(e)) {
                    render_map_main(MAIN_REN, PLAYER.x, PLAYER.y);
                }
            }
        }

        cleanup(MAIN_REN, MAIN_WIN);
    }

    SDL_Quit();
}
