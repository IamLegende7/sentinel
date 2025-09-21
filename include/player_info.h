#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include "enemy.h"

inline int ZOOM = 1;
inline int PLAYER_X = 0;  // FIXME Currently the player is in the top left corner. Fix with a offset!
inline int PLAYER_Y = 0;

//TODO: put in class
inline int PLAYER_WALK_SPEED_MAX = 10;  // modifier to base unit speed
inline int PLAYER_RUNN_SPEED_MAX = 15;  // modifier to base unit speed
inline int PLAYER_WALK_ACCELERATION = 15;  // modifier to base unit acceleration
inline int PLAYER_RUNN_ACCELERATION = 15;  // modifier to base unit acceleration
inline Unit PLAYER;

#endif