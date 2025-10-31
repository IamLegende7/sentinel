#ifndef COMBAT_HPP
#define COMBAT_HPP

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

#include "main.h"
#include "player_info.h"
#include "locations.hpp"
#include "info.h"
#include "debug.hpp"
#include "texture_agent.hpp"
#include "render_map.hpp"
#include "helper_utils.h"

class CombatLoop {
    private:
        SDL_Texture* background_texture;                    // the background (the main map) as a single texture
        SDL_Renderer* background_ren = nullptr;
        int old_x = 0;
        int old_y = 0;
        SDL_Renderer* combat_renderer;
    public:
        // Vars //
        Map* main_map;                                      // the main map class handeling tiles
        // Init and cleanup //
        CombatLoop(std::string map_name, SDL_Renderer* renderer);
        ~CombatLoop();

        // Helper functions //
        bool make_background_texture(SDL_Renderer* renderer);

        // Main fuctions //
        bool tick();
};

#endif