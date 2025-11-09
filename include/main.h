#ifndef MAIN_H
#define MAIN_H

#include <string>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_gpu.h>

#include "helper_utils.h"

/* this is where most functions are included

*/

int setup();

// Rendering //
void cleanup(SDL_Renderer* renderer, SDL_Window* window);

// Movement & Input //
void inputs_player(const SDL_Event& e);
XY init_move();

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

// GPU //
inline SDL_GPUDevice* GPU = nullptr;

// 0: main menu; 1: in game, out of combat; 2: in combat, in the map; 3: pause menu // FIXME: whats the porpose of 1?
inline int MODE = 2;  // TODO: when main menu is added: change to 0

inline XY CAMERA_POS = {0, 0};

inline int SCREEN_WIDTH = 640;
inline int SCREEN_HEIGHT = 480;

inline float RENDER_OFFSET_X = 0;
inline float RENDER_OFFSET_Y = 0;

#endif