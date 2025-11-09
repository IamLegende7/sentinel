#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include "unit.hpp"

inline float ZOOM   = 0.75;

inline int PLAYER_WALK_SPEED_MAX = 1;  // modifier to base unit speed
inline int PLAYER_RUN_SPEED_MAX = 1;  // modifier to base unit speed
inline int PLAYER_WALK_ACCELERATION = 1;  // modifier to base unit acceleration
inline int PLAYER_RUN_ACCELERATION = 1;  // modifier to base unit acceleration
inline Unit* PLAYER;

struct KeyState {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;

    bool key_pressed() {
        return up || down || left || right;
    }
};

inline KeyState KEY_STATE;

#endif