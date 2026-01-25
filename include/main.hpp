#ifndef SETUP_HPP
#define SETUP_HPP

struct XY {
    float x = 0;
    float y = 0;
};

// Windows, Renderer & GPU devices //
static SDL_Window* WINDOW = nullptr;
static SDL_Renderer* RENDERER = nullptr;
static SDL_GPUDevice* GPU = nullptr;

// Frame rate //
const int TARGET_FPS = 60;
const float TIME_STEP = 1.0f / TARGET_FPS;

// 0: main menu; 1: in game, out of combat; 2: in combat, in the map; 3: pause menu // FIXME: whats the porpose of 1?
inline int MODE = 2;  // TODO: when main menu is added: change to 0

inline int SCREEN_WIDTH = 640;
inline int SCREEN_HEIGHT = 480;

#endif