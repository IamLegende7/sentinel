#ifndef MAIN_H
#define MAIN_H

#include <string>

// this is where most functions are included

int setup();
// rendering
void cleanup(SDL_Renderer* renderer, SDL_Window* window);
int render_map_inital(std::string map_name, SDL_Renderer* renderer);
int render_map_main(SDL_Renderer* renderer, int player_x, int player_y);
// movement & input
bool movement_player(const SDL_Event& e);
//int controls_keyboard(const SDL_Event& e);


inline SDL_Window* MAIN_WIN = nullptr;
inline SDL_Renderer* MAIN_REN = nullptr;

// 0: main menu; 1: in game, out of combat; 2: in "combat", in the map; 3: pause menu
inline int MODE = 2;  // TODO: when main menu is added: change to 0

inline int SCREEN_WIDTH = 640;
inline int SCREEN_HEIGHT = 480;

#endif