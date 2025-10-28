#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "main.h"
#include "enemy.h"
#include "player_info.h"
#include "combat.hpp"

int main(int argc, char *argv[]) {
    if ( !setup() ) {
        float accumulator = 0.0f;
        Uint64 previous = SDL_GetTicks();

        // Smaller vars //
        bool reload_pressed = false;

        // move to a ```start_combat``` function or something
        //render_combat_inital("campain/debug");
        std::string map_name = "debug/debug1.jsonc";
        CombatLoop combat(map_name, MAIN_REN);
        PLAYER = init_player_unit(DEFAULT_SIZE_TILE * ZOOM * combat.main_map->settings.starting_pos.x, DEFAULT_SIZE_TILE * ZOOM * combat.main_map->settings.starting_pos.y);

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
                        RENDER_OFFSET_X = (SCREEN_WIDTH / 2) - 50;
                        RENDER_OFFSET_Y = (SCREEN_HEIGHT / 2) - 50;
                        SDL_SetRenderViewport(MAIN_REN, NULL);
                        if (MODE == 2) {
                            NEED_MAP_UPDATE = true;
                        }
                    }
                    if (MODE == 2) {
                        if ((e.key.scancode == SDL_SCANCODE_R) and DEBUG_CAN_RELOAD) { // TODO: this is just a quick-and-dirty fix for now
                            if ((e.type == SDL_EVENT_KEY_DOWN) and not reload_pressed) {
                                reload_pressed = true;
                                LOGGER.log(LogLevel::INFO, "Reloading the Map...");
                                CombatLoop combat(map_name, MAIN_REN);
                                PLAYER = init_player_unit(DEFAULT_SIZE_TILE * ZOOM * combat.main_map->settings.starting_pos.x, DEFAULT_SIZE_TILE * ZOOM * combat.main_map->settings.starting_pos.y);
                            } else if (e.type == SDL_EVENT_KEY_UP) {
                                reload_pressed = false;
                            }
                        }
                        inputs_player(e);
                    }
                }
                if (MODE == 2) {
                    move_player();
                }
                accumulator -= TIME_STEP;
            }

            // RENDERING //
            if (MODE == 2) {
                CAMERA_POS.x = PLAYER.x;
                CAMERA_POS.y = PLAYER.y;
                combat.tick();
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
