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
        bool running = false;               // true = running, false = walking
        int walk_speed_max = 10;            // maximum walk speed
        int run_speed_max = 20;             // maximum run speed
        float walk_acceleration = 0.5;      // ```walk_acceleration``` and ```run_acceleration``` are not 1:1
        float run_acceleration = 0.8;       // ```walk_acceleration``` and ```run_acceleration``` are not 1:1
        float speed_x = 0;                  // speed in x direction
        float speed_y = 0;                  // speed in y direction
        int t_up = 0;                       // The ticks that this unit has been moving is this direction
        int t_down = 0;                     // The ticks that this unit has been moving is this direction
        int t_left = 0;                     // The ticks that this unit has been moving is this direction
        int t_right = 0;                    // The ticks that this unit has been moving is this direction
        int t_up_old = 0;                   // The ticks that this unit has been moving is this direction before stopping to do so
        int t_down_old = 0;                 // The ticks that this unit has been moving is this direction before stopping to do so
        int t_left_old = 0;                 // The ticks that this unit has been moving is this direction before stopping to do so
        int t_right_old = 0;                // The ticks that this unit has been moving is this direction before stopping to do so
        int slow_down_ticks = 10;            // The ticks that this unit needs to slow down
        int directional_modifier_x = 1;     // -1 if moving left, 1 if moving right
        int directional_modifier_y = 1;     // -1 if moving up, 1 if moving down
        int step_y = 0;
        int step_x = 0;

        int control_res = 0;  // ```-1```, if impossible
        bool is_player = false;

        /* This unit becomes controlled by the player by calling this function
        
        */
        void become_player() {
            is_player = true;
        }

        void loose_player() {
            is_player = false;
            control_res = -1;
        }

        void move(XY direction) {
            // slow down
            if (direction.y != -1 && t_up != 0 ) {
                if (t_up_old == 0) { t_up_old = t_up; }
                if (direction.y == 0) {
                    t_up -= ceil(t_up_old / slow_down_ticks);
                } else {
                    t_up -= ceil(t_up_old / slow_down_ticks) * 2;       // slow down faster if going in oppesite direction
                }
                if (t_up <= 0) { t_up = 0; t_up_old = 0; }
            }
            if (direction.y != 1 && t_down != 0 ) {
                if (t_down_old == 0) { t_down_old = t_down; }
                if (direction.y == 0) {
                    t_down -= ceil(t_down_old / slow_down_ticks);
                } else {
                    t_down -= ceil(t_down_old / slow_down_ticks) * 2;       // slow down faster if going in oppesite direction
                }
                if (t_down <= 0) { t_down = 0; t_down_old = 0; }
            }
            if (direction.x != -1 && t_left != 0 ) {
                if (t_left_old == 0) { t_left_old = t_left; }
                if (direction.x == 0) {
                    t_left -= ceil(t_left_old / slow_down_ticks);
                } else {
                    t_left -= ceil(t_left_old / slow_down_ticks) * 2;       // slow down faster if going in oppesite direction
                }
                if (t_left <= 0) { t_left = 0; t_left_old = 0; }
            }
            if (direction.x != 1 && t_right != 0 ) {
                if (t_right_old == 0) { t_right_old = t_right; }
                if (direction.x == 0) {
                    t_right -= ceil(t_right_old / slow_down_ticks);
                } else {
                    t_right -= ceil(t_right_old / slow_down_ticks) * 2;       // slow down faster if going in oppesite direction
                }
                if (t_right <= 0) { t_right = 0; t_right_old = 0; }
            }

            if (!running) {
                // increment t
                if (direction.y == -1 && t_down == 0) {
                    t_up_old = 0;
                    t_up += 1;
                }
                if (direction.y == 1 and t_up == 0) {
                    t_down_old = 0;
                    t_down += 1;
                }
                if (direction.x == -1 and t_right == 0) {
                    t_left_old = 0;
                    t_left += 1;
                }
                if (direction.x == 1 and t_left == 0) {
                    t_right_old = 0;
                    t_right += 1;
                }
                // add speed
                if (direction.x == 0) { speed_y = walk_acceleration * pow(t_up + t_down, 2); }           // diagonal == false
                else                  { speed_y = walk_acceleration * pow(t_up + t_down, 2) / sqrt(2); } // diagonal == true
                if (direction.y == 0) { speed_x = walk_acceleration * pow(t_right+ t_left, 2); }
                else                  { speed_x = walk_acceleration * pow(t_right + t_left, 2) / sqrt(2); }
                if (speed_y > walk_speed_max) { speed_y = walk_speed_max; }                     // enforce max speed cap
                if (speed_x > walk_speed_max) { speed_x = walk_speed_max; }
            } else {
                printf("NO RUNNING LOGIC YET!\n");
            }
            // move
            if (t_up != 0) {
                directional_modifier_y = -1;
            } else if (t_down != 0) {
                directional_modifier_y = 1;
            }
            if (t_left != 0) {
                directional_modifier_x = -1;
            } else if (t_right != 0) {
                directional_modifier_x = 1;
            }
            step_y = ceil(speed_y) * directional_modifier_y;
            step_x = ceil(speed_x) * directional_modifier_x;
            if (step_y != 0 || step_x != 0) {
                y += step_y;
                x += step_x;
                NEED_MAP_UPDATE = true;
            }
        }
};

Unit init_player_unit(int x_pos, int y_pos);

#endif