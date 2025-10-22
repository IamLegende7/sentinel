#ifndef MAIN_H
#define MAIN_H

#include <string>

#include "helper_utils.h"

/* this is where most functions are included

*/

int setup();

// Rendering //
void cleanup(SDL_Renderer* renderer, SDL_Window* window);

// Movement & Input //
void inputs_player(const SDL_Event& e);
void move_player();

/*
    SOME VARS BELOW
*/

// Renderring //
inline int DEFAULT_SIZE_TILE = 100;

// Time & Frames //
const int TARGET_FPS = 60;
const float TIME_STEP = 1.0f / TARGET_FPS;

// Renderers & Windows //
inline SDL_Window* MAIN_WIN = nullptr;
inline SDL_Renderer* MAIN_REN = nullptr;
// inline SDL_Renderer* background_ren = nullptr;  // the render for making the map texture

// 0: main menu; 1: in game, out of combat; 2: in "combat", in the map; 3: pause menu
inline int MODE = 2;  // TODO: when main menu is added: change to 0

inline int SCREEN_WIDTH = 640;
inline int SCREEN_HEIGHT = 480;

#endif