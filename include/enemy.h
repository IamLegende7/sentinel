#ifndef ENEMY_H
#define ENEMY_H

#include <cmath>

#include "helper_utils.h"

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
        bool running = false;  // true = running, false = walking
        int walk_speed_max = 10;
        int run_speed_max = 20;
        float walk_acceleration = 0.5;  // ```walk_acceleration``` and ```run_acceleration``` are not 1:1
        float run_acceleration = 0.8;   // ```walk_acceleration``` and ```run_acceleration``` are not 1:1
        float speed_x = 0;
        float speed_y = 0;
        int t_up = 0; // The time that the player has been moving is this direction
        int t_down = 0; // The time that the player has been moving is this direction
        int t_left = 0; // The time that the player has been moving is this direction
        int t_right = 0; // The time that the player has been moving is this direction

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
            // increment / reset the timers
            if (direction.y != -1) { t_up = 0; } else { t_up += 1; }
            if (direction.y != 1) { t_down = 0; } else { t_down += 1; }
            if (direction.x != -1) { t_left = 0; } else { t_left += 1; }
            if (direction.x != 1) { t_right = 0; } else { t_right += 1; }
            if (!running) {
                speed_x = walk_acceleration * pow(t_left + t_right, 2);
                if (speed_x > walk_speed_max) {
                    speed_x = walk_speed_max;
                }
                x += direction.x * ceil(speed_x);
                speed_y = walk_acceleration * pow(t_up + t_down, 2);
                if (speed_y > walk_speed_max) {
                    speed_y = walk_speed_max;
                }
                y += direction.y * ceil(speed_y);
                printf("X: %d  Y: %d\n", x, y);
            } else {
                printf("NO RUNNING LOGIC YET!\n");
            }
        }
};

Unit init_player_unit(int x_pos, int y_pos);

#endif