#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include "main.h"
#include "player_info.h"
#include "locations.h"
#include "info.h"
#include "debug.h"

#include "texture_agent.hpp"
#include "render_map.hpp"

/*
    ### VARIBLES ###
*/
Map* main_map;                                       // the main map class handeling tiles
SDL_Texture* background_texture;                    // the background (the main map) as a single texture
SDL_Renderer* background_ren = nullptr;
int old_x = 0;
int old_y = 0;

/*  This function bakes all tiles of the map into one texture
    ### Inputs: 
    ```SLD_Renderer``` for renderring the texture. Can be the main renderer.
    ```camera_x```, ```camera_y``` for the locations of wich to render
*/
bool make_background_texture(SDL_Renderer* renderer, int camera_x, int camera_y) {
    SDL_DestroyTexture(background_texture);
    background_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderTarget(renderer, background_texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);   // Black background
    SDL_RenderClear(renderer);
    main_map->render_map(camera_x, camera_y);
    SDL_SetRenderTarget(renderer, nullptr);
    return true;
}

bool render_combat_inital(std::string map_name) {
    main_map = new Map(map_name, MAIN_REN);
    return true;
}

bool render_combat(SDL_Renderer* renderer, int camera_x, int camera_y) {

    // background (a.k.a the map)
    SDL_FRect full_window_rect = { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
    if ((old_x != camera_x) or (old_y != camera_y) or NEED_MAP_UPDATE) {
        make_background_texture(MAIN_REN, camera_x - (SCREEN_WIDTH / 2) + 50, camera_y - (SCREEN_HEIGHT / 2) + 50);
        NEED_MAP_UPDATE = false;
        old_x = camera_x;
        old_y = camera_y;
    }
    SDL_RenderTexture(renderer, background_texture, NULL, &full_window_rect);
    
    if (DEBUG_SHOW_CROSSHAIR) {
        std::string picture_path_full = TEXTURE_DIR + "/crosshair_debug.png";
        SDL_Texture* crosshair_texture = IMG_LoadTexture(renderer, picture_path_full.c_str());
        SDL_SetTextureScaleMode(crosshair_texture, SDL_SCALEMODE_NEAREST);
        SDL_FRect crosshair_rect = { (float)(ceil(SCREEN_WIDTH / 2) - (DEFAULT_SIZE_TILE * ZOOM / 3)), (float)((SCREEN_HEIGHT / 2) - (DEFAULT_SIZE_TILE * ZOOM / 3)), (float)(DEFAULT_SIZE_TILE * ZOOM / 2), (float)(DEFAULT_SIZE_TILE * ZOOM / 2) };
        SDL_RenderTexture(MAIN_REN, crosshair_texture, NULL, &crosshair_rect);
    }

    if (DEBUG_SHOW_HITBOXES) {
        // for (Unit current_unit : UNITS) {
            // moveboxes
            SDL_SetRenderDrawColor(MAIN_REN, 255, 0, 255, 255);  // set coulor to pink/magenta/whatever
            SDL_FRect hitbox_rect = { (float)(SCREEN_WIDTH / 2) - 50, (float)(SCREEN_HEIGHT / 2) - 50, (float)PLAYER.move_box.width, (float)PLAYER.move_box.height };
            SDL_RenderRect(MAIN_REN, &hitbox_rect);
    }

    // render debug information   // TODO: change to actual text, not SDL debug text
    SDL_SetRenderDrawColor(MAIN_REN, 255, 255, 255, 255);  // set coulor to white
    if (DEBUG_SHOW_COORDS) {
        std::ostringstream coords;
        coords << "X: " << PLAYER.x << " Y: " << PLAYER.y;
        SDL_RenderDebugText(MAIN_REN, 5, 5, coords.str().c_str());
    }
    if (DEBUG_SHOW_SPEED) {
        std::ostringstream speed;
        speed << "SpeedY: " << PLAYER.speed_y << " SpeedX: " << PLAYER.speed_x;
        SDL_RenderDebugText(MAIN_REN, 5, 20, speed.str().c_str());
    }
    return true;
}