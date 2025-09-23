#ifndef MAIN_H
#define MAIN_H

#include <string>

// this is where most functions are included

int setup();
// rendering
void cleanup(SDL_Renderer* renderer, SDL_Window* window);
// movement & input
void inputs_player(const SDL_Event& e);
bool move_player();
//int controls_keyboard(const SDL_Event& e);


const int TARGET_FPS = 60;
const float TIME_STEP = 1.0f / TARGET_FPS;

inline SDL_Window* MAIN_WIN = nullptr;
inline SDL_Renderer* MAIN_REN = nullptr;
// inline SDL_Renderer* background_ren = nullptr;  // the render for making the map texture

// 0: main menu; 1: in game, out of combat; 2: in "combat", in the map; 3: pause menu
inline int MODE = 2;  // TODO: when main menu is added: change to 0

inline int SCREEN_WIDTH = 640;
inline int SCREEN_HEIGHT = 480;

#endif