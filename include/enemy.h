#ifndef ENEMY_H
#define ENEMY_H

#include <cmath>

#include "helper_utils.h"
#include "debug.h"
#include "logger.hpp"
#include "combat_vars_one.hpp"

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

        // colliding
        Hitbox movebox = {1, 0, 0, 0, 80, 100, 20}; // collides with walls and such
        int stuck_in_wall = 0; // ticks the player was stuck in a wall; if too high: apply backwards force

        // walking stuff
        int to_move = 0;
        float slow_down = 1;                // how quickly the player loses speed, when slowing down
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


        // stats
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

        void tick() {
            if (!is_player) {
                // unit ai here
                XY direction = {0, 0}; // delete later
                move(direction);
            }
        }

        void move(XY direction) {
            if (!running) { 
                /////////////
                // Walking //
                /////////////
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
            } else {
                /////////////
                // Running //
                /////////////
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
                    speed_x -= slow_down * 2; //
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

            //////////////
            // MOVEMENT //
            //////////////
            float diagonal_compensator_x;
            float diagonal_compensator_y;
            if (direction.y == 0) diagonal_compensator_x = 1;             // diagonal = false
            else                  diagonal_compensator_x = sqrt(2);       // diagonal = true
            if (direction.x == 0) diagonal_compensator_y = 1;
            else                  diagonal_compensator_y = sqrt(2);

            /// COLLISION ///
            bool colliding_x = false;
            bool colliding_y = false;
            // X //
            Hitbox test_movebox = movebox;
            test_movebox.x += ceil(speed_x / diagonal_compensator_x) * directional_modifier_x;
            for (auto& tile_movebox_vector : MOVEBOXES_TILES) {
                for (auto& tile_movebox : tile_movebox_vector) {
                    if (test_movebox.colliding(tile_movebox)) {
                        colliding_x = true;
                        stuck_in_wall++;
                        speed_x = 0;
                        break;
                    }
                }
                if (colliding_x) break;
            }

            // Y //
            test_movebox = movebox;
            test_movebox.y += ceil(speed_y / diagonal_compensator_y) * directional_modifier_y;
            for (auto& tile_movebox_vector : MOVEBOXES_TILES) {
                for (auto& tile_movebox : tile_movebox_vector) {
                    if (test_movebox.colliding(tile_movebox)) {
                        colliding_y = true;
                        stuck_in_wall++;
                        speed_y = 0;
                        break;
                    }
                }
                if (colliding_y) break;
            }


            // Unstuck //
            /* Sometimes the unit can get stuck in corners.
               This code makes shure you will be pushed outside the hitbox.
            */
            if (!(colliding_x) and !(colliding_y)) stuck_in_wall = 0;
            if (DEBUG_ANNOYING_LOGS and (stuck_in_wall != 0)) LOGGER.log(LogLevel::DEBUG, "stuck_in_wall: %d", stuck_in_wall);
            bool still_colliding;
            if (stuck_in_wall >= 15) {
                if (colliding_x) {
                    still_colliding = false;
                    // LEFT //
                    test_movebox = movebox;
                    test_movebox.x -= 20;
                    for (auto& tile_movebox_vector : MOVEBOXES_TILES) {
                        for (auto& tile_movebox : tile_movebox_vector) { if (test_movebox.colliding(tile_movebox)) still_colliding = true; }
                        if (still_colliding) break;
                    }
                    if (!still_colliding) { x -= 1; }
                    // RIGHT //
                    if (still_colliding) {
                        test_movebox = movebox;
                        test_movebox.x += 20;
                        for (auto& tile_movebox_vector : MOVEBOXES_TILES) {
                            for (auto& tile_movebox : tile_movebox_vector) { if (test_movebox.colliding(tile_movebox)) still_colliding = true; }
                            if (still_colliding) break;
                        }
                        if (!still_colliding) { x += 1; }
                    }
                }
                if (colliding_y) {
                    still_colliding = false;
                    // UP //
                    test_movebox = movebox;
                    test_movebox.y -= 20;
                    for (auto& tile_movebox_vector : MOVEBOXES_TILES) {
                        for (auto& tile_movebox : tile_movebox_vector) { if (test_movebox.colliding(tile_movebox)) still_colliding = true; }
                        if (still_colliding) break;
                    }
                    if (!still_colliding) { y -= 1; }
                    // DOWN //
                    if (still_colliding) {
                        test_movebox = movebox;
                        test_movebox.y += 20;
                        for (auto& tile_movebox_vector : MOVEBOXES_TILES) {
                            for (auto& tile_movebox : tile_movebox_vector) { if (test_movebox.colliding(tile_movebox)) still_colliding = true; }
                            if (still_colliding) break;
                        }
                        if (!still_colliding) { y += 1; }
                    }
                }
                movebox.x = x + movebox.x_offset;
                movebox.y = y + movebox.y_offset;
            }


            /// MOVE ///
            if ((direction.y == 0) or colliding_y) diagonal_compensator_x = 1;             // diagonal = false
            else                                   diagonal_compensator_x = sqrt(2);       // diagonal = true
            if ((direction.x == 0) or colliding_x) diagonal_compensator_y = 1;
            else                                   diagonal_compensator_y = sqrt(2);

            // X //
            if (speed_x != 0) {
                if (!colliding_x) {
                    x += ceil(speed_x / diagonal_compensator_x) * directional_modifier_x;
                    movebox.x = x + movebox.x_offset;
                }
            }

            // Y //
            if (speed_y != 0) {
                if (!colliding_y) {
                    y += ceil(speed_y / diagonal_compensator_x) * directional_modifier_y;
                    movebox.y = y + movebox.y_offset;
                }
            }
        }
};

Unit init_player_unit(int x_pos, int y_pos);

#endif