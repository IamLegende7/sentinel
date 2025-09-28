#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

#include "main.h"
#include "enemy.h"
#include "player_info.h"
#include "render_map.h"

int main(int argc, char *argv[]) {
    if ( !setup() ) {
        float accumulator = 0.0f;
        Uint64 previous = SDL_GetTicks();

        // move to a ```start_combat``` function or something
        render_combat_inital("campain/debug");
        PLAYER = init_player_unit(0, 0);

        // main game loop
        bool quit;
        SDL_Event e;
        while (!quit) {
            Uint64 current = SDL_GetTicks();
            float elapsed = (current - previous) / 1000.0f;
            previous = current;
            accumulator += elapsed;

            // Process updates at a fixed rate
            while (accumulator >= TIME_STEP) {
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_EVENT_QUIT) quit = true;
                    if (e.type == SDL_EVENT_WINDOW_RESIZED) {
                        SCREEN_WIDTH = e.window.data1;
                        SCREEN_HEIGHT = e.window.data2;
                        SDL_SetRenderViewport(MAIN_REN, NULL);
                        if (MODE == 2) {
                            NEED_MAP_UPDATE = true;
                        }
                    }
                    if (MODE == 2) {
                        inputs_player(e);
                    }
                }
                if (MODE == 2) {
                    move_player();
                }
                accumulator -= TIME_STEP;
            }

            //rendering here vv
            SDL_SetRenderDrawColor(MAIN_REN, 0, 0, 0, 255);
            if (MODE == 2) {
                render_combat(MAIN_REN, PLAYER.x, PLAYER.y);
            }
            SDL_RenderPresent(MAIN_REN);


            float sleepTime = (1.0f / TARGET_FPS) - accumulator;
            if (sleepTime > 0) {
            SDL_Delay((int)(sleepTime * 1000));
        }

        }

        cleanup(MAIN_REN, MAIN_WIN);
    }

    SDL_Quit();
}
