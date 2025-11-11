#ifndef UNIT_HPP
#define UNIT_HPP

#include <cmath>

#include "utils/helper_utils.h"
#include "utils/logger.hpp"
#include "utils/hitboxes.hpp"

#include "settings/debug.hpp"
#include "settings/locations.hpp"

struct Effect {
    std::string id;
    int duration;
    int strength;
};

/* This class manages units (*ALL* units: player, humans, aliens, vehicles, etc.)

*/
class Unit {
    private:
        // Varibles //
            // Walking Stuff
            int t_x = 0, t_y = 0;
            XY directional_modifier;

        void move(XY direction);

    public:
        // Init & Cleanup //
        Unit(std::string id = "sentinel:none", nlohmann::json properties = nlohmann::json{}, XY starting_pos = {0, 0});
        ~Unit();
        // Varibles //
            // General
            int x = 0, y = 0;
            bool is_player = false;
            std::vector<Effect> effects;
            std::vector<Hitbox> moveboxes;
            std::vector<Hitbox> hitboxes;

            // Stats
            int hp_max = 1;
            int hp = 1;

            int control_resistance = 0;

            // Walking Stuff
            XY speed;
            bool running = false;
            float slow_down = 1;
            float walk_acceleration = 0;
            float run_acceleration = 0;
            int walk_speed_max = 0;
            int run_speed_max = 0;

        // Ticking //
        void tick();

        // Player //
        void become_player();
        void loose_player();
};

#endif