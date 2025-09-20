#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

#include "main.h"
#include "input.h"

int main(int argc, char *argv[]) {
    if ( !setup() ) {
        render_map_inital("campain/debug", MAIN_REN);
        render_map_main(MAIN_REN);
        // main game loop
        bool quit;
        SDL_Event e;
        while (!quit) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_EVENT_QUIT) quit = true;
                if (e.type == SDL_EVENT_KEY_DOWN) {
                    
                }
                if (e.type == SDL_EVENT_WINDOW_RESIZED) {
                    SCREEN_WIDTH = e.window.data1;   // laggy for some reason
                    SCREEN_HEIGHT = e.window.data2;
                    SDL_SetRenderViewport(MAIN_REN, NULL);
                    render_map_main(MAIN_REN);
                }
            }

            // render only if nessesary
            if (movement_player()) {
                render_map_main(MAIN_REN);
            }
        }

        cleanup(MAIN_REN, MAIN_WIN);
    }

    SDL_Quit();
}
