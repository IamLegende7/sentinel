#include "loops/combat.hpp"
#include "utils/hitboxes.hpp"

CombatLoop::CombatLoop(std::string map_name, SDL_Renderer* renderer) {
    combat_renderer = renderer;
    main_map = new Map(map_name, combat_renderer);
}

CombatLoop::~CombatLoop() {
    delete main_map;
    main_map = nullptr;
}

/*  This function bakes all tiles of the map into one texture
    ### Inputs: 
    ```SLD_Renderer``` for renderring the texture. Can be the main renderer.
*/
bool CombatLoop::make_background_texture(SDL_Renderer* renderer) {
    SDL_DestroyTexture(background_texture);
    background_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderTarget(renderer, background_texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);   // Black background
    SDL_RenderClear(renderer);
    main_map->render_map(CAMERA_POS.x - RENDER_OFFSET_X, CAMERA_POS.y - RENDER_OFFSET_Y);
    SDL_SetRenderTarget(renderer, nullptr);
    return true;
}

bool CombatLoop::tick() {
    // background (a.k.a the map)
    SDL_SetRenderDrawColor(combat_renderer, 0, 0, 0, 255);
    SDL_FRect full_window_rect = { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };

    // UPDATE MAP //
    if ((old_x != CAMERA_POS.x) or (old_y != CAMERA_POS.y) or NEED_MAP_UPDATE) {
        make_background_texture(combat_renderer);
        NEED_MAP_UPDATE = false;
        old_x = CAMERA_POS.x;
        old_y = CAMERA_POS.y;
    }
    SDL_RenderTexture(combat_renderer, background_texture, NULL, &full_window_rect);

    if (DEBUG["show_hitboxes"]) {
        MOVEBOXES->root->render_hitboxes(combat_renderer, "#CB1ED1");
        for (Hitbox& movebox : PLAYER->moveboxes) {
            movebox.render(combat_renderer, "#48ef32", true);
        }
    }

    if (DEBUG["show_relevant_hitboxes"]) {
        std::vector<Hitbox> relevant_moveboxes = MOVEBOXES->get_relevant({(float)(PLAYER->x), (float)(PLAYER->y)});
        for (Hitbox& relevant_movebox : relevant_moveboxes) {
            relevant_movebox.render(MAIN_REN, "#32C5EF");
        }
    }

    if (DEBUG["show_nodes"]) {
        MOVEBOXES->root->render("#ffffff");
    }

    // render debug information   // TODO: change to actual text, not SDL debug text
    SDL_SetRenderDrawColor(combat_renderer, 255, 255, 255, 255);  // set coulor to white
    if (DEBUG["show_coords"]) {
        std::ostringstream coords;
        coords << "X: " << PLAYER->x << " Y: " << PLAYER->y;
        SDL_RenderDebugText(combat_renderer, 5, 5, coords.str().c_str());
    }
    if (DEBUG["show_speed"]) {
        std::ostringstream speed;
        speed << "SpeedY: " << PLAYER->speed.y << " SpeedX: " << PLAYER->speed.x;
        SDL_RenderDebugText(combat_renderer, 5, 20, speed.str().c_str());
    }
    return true;
}