/*  In this file are the main scrips and functions for enemys.

    Please note that the player is just an enemy with deactivated AI.
    Thous quite a lot of player-handeling gets done here too.

*/

#include <SDL3/SDL.h>
#include <stdio.h>

#include "main.h"
#include "enemy.h"

// ## temporary
Unit init_player_unit(int x_pos, int y_pos) { // TODO: remove this later
    Unit player;
    player.hp_max = 1000;
    player.hp = player.hp_max;
    player.x = x_pos;
    player.y = y_pos;
    player.become_player();
    return player;
}