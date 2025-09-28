#include <SDL3/SDL.h>
#include <stdio.h>

#include "main.h"
#include "player_info.h"
#include "enemy.h"
#include "helper_utils.h"

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

XY init_move() {
    XY direction;
    direction.x = 0;
    direction.y = 0;
    if (KEY_STATE.key_pressed()) {
        if (KEY_STATE.up) {
            direction.y--;
        }
        if (KEY_STATE.down) {
            direction.y++;
        }
        if (KEY_STATE.left) {
            direction.x--;
        }
        if (KEY_STATE.right) {
            direction.x++;
        }
    }
    return direction;
}

/* ### Move the player
    Returns true if the Player moved



*/
void inputs_player(const SDL_Event& e) {
    SDL_PumpEvents();
    if (e.type == SDL_EVENT_KEY_DOWN) {  // TODO: add keybinds
        switch (e.key.scancode) {
            case SDL_SCANCODE_W: 
                KEY_STATE.up = true;
                break;
            case SDL_SCANCODE_S: 
                KEY_STATE.down = true;
                break;
            case SDL_SCANCODE_A: 
                KEY_STATE.left = true;
                break;
            case SDL_SCANCODE_D: 
                KEY_STATE.right = true;
                break;
            case (SDL_SCANCODE_LCTRL): 
                PLAYER.running = true;
                break;
            default:
                break;
        }
    }
    if (e.type == SDL_EVENT_KEY_UP) {
        switch (e.key.scancode) {
            case SDL_SCANCODE_W:
                KEY_STATE.up = false;
                break;
            case SDL_SCANCODE_S:
                KEY_STATE.down = false;
                break;
            case SDL_SCANCODE_A: 
                KEY_STATE.left = false;
                break;
            case SDL_SCANCODE_D:
                KEY_STATE.right = false;
                break;
            case (SDL_SCANCODE_LCTRL): 
                PLAYER.running = false;
                break;
            default:
                break;
        }
    }
}
    
void move_player() {
    XY move_direction = init_move();
    PLAYER.move(move_direction);
    PLAYER.move_box.x = PLAYER.x + PLAYER.move_box.x_offset;
    PLAYER.move_box.y = PLAYER.y + PLAYER.move_box.y_offset;
}
// PLAYER_SPEED_Y = PLAYER_SPEED_Y + PLAYER_BASE_SPEED;