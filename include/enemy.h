#ifndef ENEMY_H
#define ENEMY_H

#include <cmath>

#include "helper_utils.h"
#include "render_map.h"

struct Effect {
    std::string id;
    int duration;
    int strength;
};

/* This class manages uints (such as the enemys or the player)

*/
class Unit {
    public:
        int x = 0;
        int y = 0;
        int hp_max = 1;
        int hp = 1;

        // walking stuff
        float slow_down = 1;                // how quickly the player loses speed, when slowing down // TODO: make slowing down when sprinting slower, but faster, when key in other direction
        bool running = false;               // true = running, false = walking
        int walk_speed_max = 10;            // maximum walk speed
        int run_speed_max = 17;             // maximum run speed
        float walk_acceleration = 0.3;      // ```walk_acceleration``` and ```run_acceleration``` are not 1:1
        float run_acceleration = 0.1;       // ```walk_acceleration``` and ```run_acceleration``` are not 1:1
        float speed_x = 0;                  // speed in x direction
        float speed_y = 0;                  // speed in y direction
        int t_y = 0;
        int t_x = 0;
        int directional_modifier_y = 0;
        int directional_modifier_x = 0;

        int control_resistance = 0;  // ```-1```, if impossible to control
        bool is_player = false;

        /* This unit becomes controlled by the player by calling this function
        
        */
        void become_player() {
            is_player = true;
        }

        void loose_player() {
            is_player = false;
            control_resistance = -1;
        }

        void move(XY direction) {
            if (!running) { // Walking
                // Y
                if (directional_modifier_y == 0) {
                    directional_modifier_y = direction.y;
                }
                if (directional_modifier_y != direction.y) {    // slow down
                    speed_y -= slow_down;
                    if (speed_y <= 0) {
                        speed_y = 0;
                        directional_modifier_y = 0;
                    }
                } else if (direction.y != 0) {                                      // speed up
                    t_y = ceil(sqrt(speed_y / walk_acceleration)) + 1;              // get t from the speed (& increment)
                    speed_y = walk_acceleration * pow(t_y, 2);
                    // printf("SpeedY: %f\n", speed_y);
                    if (speed_y > walk_speed_max) { speed_y = walk_speed_max; }     // enforce max speed cap
                }

                // X
                if (directional_modifier_x == 0) {
                    directional_modifier_x = direction.x;
                }
                if (directional_modifier_x != direction.x) {    // slow down
                    speed_x -= slow_down;
                    if (speed_x <= 0) {
                        speed_x = 0;
                        directional_modifier_x = 0;
                    }
                } else if (direction.x != 0) {                                      // speed up
                    t_x = ceil(sqrt(speed_x / walk_acceleration)) + 1;              // get t from the speed (& increment)
                    speed_x = walk_acceleration * pow(t_x, 2);
                    // printf("SpeedX: %f\n", speed_x);
                    if (speed_x > walk_speed_max) { speed_x = walk_speed_max; }     // enforce max speed cap
                }
            } else { // Running
                const double eulers_constant = std::exp(1.0);
                // Y
                if (directional_modifier_y == 0) {
                    directional_modifier_y = direction.y;
                }
                if (directional_modifier_y != direction.y) {    // slow down
                    speed_y -= slow_down * 2;
                    if (speed_y <= 0) {
                        speed_y = 0;
                        directional_modifier_y = 0;
                    }
                } else if (direction.y != 0) {                                                                  // speed up
                    if (speed_y < run_speed_max) {
                        t_y = ceil((-1 * log(-1 * ((speed_y / run_speed_max) - 1))) / run_acceleration) + 1;    // get t from the speed (& increment)
                    }
                    speed_y = run_speed_max * (1 - pow(eulers_constant, -1 * run_acceleration * t_y));
                    // printf("SpeedY: %f\n", speed_y);
                    if (speed_y > run_speed_max) { speed_y = run_speed_max; }                                   // enforce max speed cap
                }

                // X
                if (directional_modifier_x == 0) {
                    directional_modifier_x = direction.x;
                }
                if (directional_modifier_x != direction.x) {    // slow down
                    speed_x -= slow_down * 2;
                    if (speed_x <= 0) {
                        speed_x = 0;
                        directional_modifier_x = 0;
                    }
                } else if (direction.x != 0) {                                                                  // speed up
                    if (speed_x < run_speed_max) {
                        t_x = ceil((-1 * log(-1 * ((speed_x / run_speed_max) - 1))) / run_acceleration) + 1;    // get t from the speed (& increment)
                    }
                    speed_x = run_speed_max * (1 - pow(eulers_constant, -1 * run_acceleration * t_x));
                    // printf("SpeedX: %f\n", speed_x);
                    if (speed_x > run_speed_max) { speed_x = run_speed_max; }                                   // enforce max speed cap
                }
            }

            // move
            if (speed_y != 0 || speed_x != 0) {
                if (direction.x == 0) { y += ceil(speed_y) * directional_modifier_y; }              // diagonal = false
                else                  { y += ceil(speed_y / sqrt(2)) * directional_modifier_y; }    // diagonal = true
                if (direction.y == 0) { x += ceil(speed_x) * directional_modifier_x; }
                else                  { x += ceil(speed_x / sqrt(2)) * directional_modifier_x; }
                NEED_MAP_UPDATE = true;
            }
        }
};

Unit init_player_unit(int x_pos, int y_pos);

#endif