#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <sstream>

#include "main.h"
#include "input.h"
#include "enemy.h"
#include "player_info.h"

int main(int argc, char *argv[]) {
    if ( !setup() ) {
        render_map_inital("campain/debug", MAIN_REN);
        PLAYER = init_player_unit(0, 0);
        render_map_main(PLAYER.x, PLAYER.y);
        SDL_RenderPresent(MAIN_REN);

        float accumulator = 0.0f;
        Uint64 previous = SDL_GetTicks();

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
                        render_map_main(PLAYER.x, PLAYER.y);
                        SDL_RenderPresent(MAIN_REN);
                    }
                    inputs_player(e);
                }
                if (move_player()) {
                    render_map_main(PLAYER.x, PLAYER.y);
                        std::ostringstream oss;
                        oss << "X: " << PLAYER.x << " Y: " << PLAYER.y;
                        std::string coords = oss.str();
                    SDL_SetRenderDrawColor(MAIN_REN, 255, 255, 255, 255);
                    SDL_RenderDebugText(MAIN_REN, 5, 5, coords.c_str());
                    SDL_RenderPresent(MAIN_REN);
                }
                accumulator -= TIME_STEP;
            }

            //rendering here vv

            float sleepTime = (1.0f / TARGET_FPS) - accumulator;
            if (sleepTime > 0) {
            SDL_Delay((int)(sleepTime * 1000));
        }

        }

        cleanup(MAIN_REN, MAIN_WIN);
    }

    SDL_Quit();
}
