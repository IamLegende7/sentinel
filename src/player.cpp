#include <SDL3/SDL.h>
#include <stdio.h>

#include "main.h"
#include "player_info.h"
#include "enemy.h"
#include "helper_utils.h"

struct KeyState {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;

    bool key_pressed() {
        return up || down || left || right;
    }
};

struct MoveState {
    int x_direction = 0;
    int y_direction = 0;

    bool moving() {
        if (!x_direction == 0 and !y_direction == 0) {
            return true;
        } else {
            return false;
        }
    }
};

XY init_move(KeyState key_state) {
    XY direction;
    direction.x = 0;
    direction.y = 0;
    if (key_state.up) {
        direction.y--;
    }
    if (key_state.down) {
        direction.y++;
    }
    if (key_state.left) {
        direction.x--;
    }
    if (key_state.right) {
        direction.x++;
    }
    return direction;
}

/* ### Move the player
    Returns true if the Player moved



*/
bool movement_player(const SDL_Event& e) {
    SDL_PumpEvents();
    KeyState key_state;
    //const bool* state = SDL_GetKeyboardState(NULL);
    if (e.type == SDL_EVENT_KEY_DOWN) {  // TODO: add keybinds
        switch (e.key.scancode) {
            case SDL_SCANCODE_W: 
                key_state.up = true;
                break;
            case SDL_SCANCODE_S: 
                key_state.down = true;
                break;
            case SDL_SCANCODE_A: 
                key_state.left = true;
                break;
            case SDL_SCANCODE_D: 
                key_state.right = true;
                break;
            default:
                break;
            // case (SDL_SCANCODE_LCTRL): PLAYER_RUNNING = true; TODO: add in later
        }
    }
    if (e.type == SDL_EVENT_KEY_UP) {
        switch (e.key.scancode) {
            case SDL_SCANCODE_W:
                printf("[W] up\n");
                key_state.up = false;
                break;
            case SDL_SCANCODE_S:
                key_state.down = false;
                break;
            case SDL_SCANCODE_A: 
                key_state.left = false;
                break;
            case SDL_SCANCODE_D:
                key_state.right = false;
                break;
            default:
                break;
            // case (SDL_SCANCODE_LCTRL): PLAYER_RUNNING = false;
        }
    }
    if (key_state.key_pressed()) {
        XY move_direction = init_move(key_state);
        PLAYER.move(move_direction);

        return true;
    } else {
        return false;
    }
}
// PLAYER_SPEED_Y = PLAYER_SPEED_Y + PLAYER_BASE_SPEED;