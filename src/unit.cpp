#include "unit.hpp"

#include "player_info.h"

/*  In this file are the main scrips and functions for enemys located.

    Please note that the player is just an enemy with deactivated AI.
    Thous quite a lot of player-handeling gets done here too.

*/

Unit::Unit(const std::string id, const XY starting_pos, const rapidjson::Value properties) {
    try {
        if (!open_json(LOCATIONS["units_json"]).HasMember(id.c_str())) {
            LOGGER.log(LogLevel::ERROR, "[unit.cpp:Unit] Unit not found: %s in %s", id.c_str(), std::string(LOCATIONS["units_json"]).c_str());
        } else {
            const rapidjson::Value unit_defaults = std::move(open_json(LOCATIONS["units_json"])[id.c_str()]);

            x = (int)(starting_pos.x);
            y = (int)(starting_pos.y);


            // BASIC STATS //
            if (properties.HasMember("hp_max")) hp_max = properties["hp_max"].GetInt();
            else if (unit_defaults.HasMember("hp_max")) hp_max = unit_defaults["hp_max"].GetInt();
            else LOGGER.log(LogLevel::WARNING, "[unit.cpp:Unit] No option <hp_max> in: %s", std::string(LOCATIONS["units_json"]).c_str());

            if (properties.HasMember("hp")) hp = properties["hp"].GetInt();

            if (properties.HasMember("control_resistance")) control_resistance = properties["control_resistance"].GetInt();
            else if (unit_defaults.HasMember("control_resistance")) control_resistance = unit_defaults["control_resistance"].GetInt();
            else LOGGER.log(LogLevel::WARNING, "[unit.cpp:Unit] No option <control_resistance> in: %s", std::string(LOCATIONS["units_json"]).c_str());


            // WALKING //
            if (properties.HasMember("slow_down")) slow_down = properties["slow_down"].GetFloat();
            else if (unit_defaults.HasMember("slow_down")) slow_down = unit_defaults["slow_down"].GetFloat();
            else LOGGER.log(LogLevel::WARNING, "[unit.cpp:Unit] No option <slow_down> in: %s", std::string(LOCATIONS["units_json"]).c_str());

            if (properties.HasMember("walk_acceleration")) walk_acceleration = properties["walk_acceleration"].GetFloat();
            else if (unit_defaults.HasMember("walk_acceleration")) walk_acceleration = unit_defaults["walk_acceleration"].GetFloat();
            else LOGGER.log(LogLevel::WARNING, "[unit.cpp:Unit] No option <walk_acceleration> in: %s", std::string(LOCATIONS["units_json"]).c_str());

            if (properties.HasMember("run_acceleration")) run_acceleration = properties["run_acceleration"].GetFloat();
            else if (unit_defaults.HasMember("run_acceleration")) run_acceleration = unit_defaults["run_acceleration"].GetFloat();
            else LOGGER.log(LogLevel::WARNING, "[unit.cpp:Unit] No option <run_acceleration> in: %s", std::string(LOCATIONS["units_json"]).c_str());

            if (properties.HasMember("walk_speed_max")) walk_speed_max = properties["walk_speed_max"].GetInt();
            else if (unit_defaults.HasMember("walk_speed_max")) walk_speed_max = unit_defaults["walk_speed_max"].GetInt();
            else LOGGER.log(LogLevel::WARNING, "[unit.cpp:Unit] No option <walk_speed_max> in: %s", std::string(LOCATIONS["units_json"]).c_str());

            if (properties.HasMember("run_speed_max")) run_speed_max = properties["run_speed_max"].GetInt();
            else if (unit_defaults.HasMember("run_speed_max")) run_speed_max = unit_defaults["run_speed_max"].GetInt();
            else LOGGER.log(LogLevel::WARNING, "[unit.cpp:Unit] No option <run_speed_max> in: %s", std::string(LOCATIONS["units_json"]).c_str());


            // HITBOXES //
            if (properties.HasMember("moveboxes") || unit_defaults.HasMember("moveboxes")) {
                const rapidjson::Value& moveboxes_json = 
                    properties.HasMember("moveboxes") ? 
                        properties["moveboxes"] : 
                        unit_defaults["moveboxes"];
                for (const rapidjson::Value& movebox_json_single : moveboxes_json.GetArray()) {
                    Hitbox new_movebox = {
                        movebox_json_single[0].GetInt(),
                        0, 0,
                        movebox_json_single[3].GetFloat(),
                        movebox_json_single[4].GetFloat(),
                        movebox_json_single[5].GetFloat(),
                        movebox_json_single[6].GetFloat()
                    };
                    moveboxes.push_back(new_movebox);
                }
            }

            if (properties.HasMember("hitboxes") || unit_defaults.HasMember("hitboxes")) {
                const rapidjson::Value& hitboxes_json = 
                    properties.HasMember("hitboxes") ?
                        properties["hitboxes"] : 
                        unit_defaults["hitboxes"];
                for (const rapidjson::Value& hitboxes_json_single : hitboxes_json.GetArray()) {
                    Hitbox new_hitbox = {
                        hitboxes_json_single[0].GetInt(),
                        0, 0,
                        hitboxes_json_single[3].GetFloat(),
                        hitboxes_json_single[4].GetFloat(),
                        hitboxes_json_single[5].GetFloat(),
                        hitboxes_json_single[6].GetFloat()
                    };
                    hitboxes.push_back(new_hitbox);
                }
            }

        } // if has id member
    } catch (...) {
        LOGGER.log(LogLevel::ERROR, "[unit.cpp:Unit] Could not load unit: unexpected error");
    }
} // function "Unit::Unit"

Unit::~Unit() {

}

void Unit::move(XY direction) {
    //////////////////
    // UPDATE SPEED //
    //////////////////
    if (!running) {
        /////////////
        // Walking //
        /////////////
        // X // 
        if (directional_modifier.x == 0) directional_modifier.x = direction.x;
        if (directional_modifier.x != direction.x) { // SLOW DOWN
            speed.x -= slow_down;
            if (speed.x <= 0) {
                speed.x = 0;
                directional_modifier.x = 0;
            }
        } else if (direction.x != 0) {
            t_x = ceil(sqrt(speed.x / walk_acceleration)) + 1;  // get t from the speed (& increment)
            speed.x = walk_acceleration * pow(t_x, 2);
            if (speed.x > walk_speed_max) speed.x = walk_speed_max; // Enforce speed cap
        }

        // Y //
        if (directional_modifier.y == 0) directional_modifier.y = direction.y;
        if (directional_modifier.y != direction.y) { // SLOW DOWN
            speed.y -= slow_down;
            if (speed.y <= 0) {
                speed.y = 0;
                directional_modifier.y = 0;
            }
        } else if (direction.y != 0) {
            t_y = ceil(sqrt(speed.y / walk_acceleration)) + 1;  // get t from the speed (& increment)
            speed.y = walk_acceleration * pow(t_y, 2);
            if (speed.y > walk_speed_max) speed.y = walk_speed_max; // Enforce speed cap
        }
    } else {
        const double eulers_constant = std::exp(1.0);
        // X //
        if (directional_modifier.x == 0) directional_modifier.x = direction.x;
        if (directional_modifier.x != direction.x) { // slow down
            speed.x -= slow_down * 2;
            if (speed.x <= 0) {
                speed.x = 0;
                directional_modifier.x = 0;
            }
        } else if (direction.x != 0) {
            t_x = ceil((-1 * log(-1 * ((speed.x / run_speed_max) - 1))) / run_acceleration) + 1;  // get t from the speed (& increment)
            speed.x = run_speed_max * (1 - pow(eulers_constant, -1 * run_acceleration * t_x));
            if (speed.x > run_speed_max) speed.x = run_speed_max;
        }

        // Y //
        if (directional_modifier.y == 0) directional_modifier.y = direction.y;
        if (directional_modifier.y != direction.y) { // slow down
            speed.y -= slow_down * 2;
            if (speed.y <= 0) {
                speed.y = 0;
                directional_modifier.y = 0;
            }
        } else if (direction.y != 0) {
            t_y = ceil((-1 * log(-1 * ((speed.y / run_speed_max) - 1))) / run_acceleration) + 1;  // get t from the speed (& increment)
            speed.y = run_speed_max * (1 - pow(eulers_constant, -1 * run_acceleration * t_y));
            if (speed.y > run_speed_max) speed.y = run_speed_max;
        }
    }

    ////////////////
    // CHANGE POS //
    ////////////////

    /// COLLISION ///
    if (speed.x != 0 or speed.y != 0) {
        bool is_colliding_x = false, is_colliding_y = false;
        std::vector<Hitbox> relevant_moveboxes = MOVEBOXES->get_relevant({(float)(x), (float)(y)});
        // X //
        if (speed.x != 0) {
            for (Hitbox& test_movebox : moveboxes) {
                test_movebox.x += ceil(speed.x) * directional_modifier.x;
                for (Hitbox& tile_movebox : relevant_moveboxes) {
                    if (colliding(test_movebox, tile_movebox)) {
                        is_colliding_x = true;
                        break;
                    }
                }
            }
        }

        // Y //
        if (speed.y != 0) {
            for (Hitbox& test_movebox : moveboxes) {
                test_movebox.y += ceil(speed.y) * directional_modifier.y;
                for (Hitbox& tile_movebox : relevant_moveboxes) {
                    if (colliding(test_movebox, tile_movebox)) {
                        is_colliding_y = true;
                        break;
                    }
                }
            }
        }

        float diagonal_compensator;
        if (direction.y != 0 && direction.x != 0 && !is_colliding_x && !is_colliding_y) diagonal_compensator = sqrt(2);
        else diagonal_compensator = 1;

        if (speed.x != 0) {
            if (!is_colliding_x) x += ceil(speed.x / diagonal_compensator) * directional_modifier.x;
            else speed.x = 0;
            for (Hitbox& update_movebox : moveboxes) update_movebox.x = this->x; // Update hitbox positions
            for (Hitbox& update_hitbox : hitboxes)   update_hitbox.x  = this->x;
        }

        if (speed.y != 0) {
            if (!is_colliding_y) y += ceil(speed.y / diagonal_compensator) * directional_modifier.y;
            else speed.y = 0;
            for (Hitbox& update_movebox : moveboxes) update_movebox.y = this->y; // Update hitbox positions
            for (Hitbox& update_hitbox : hitboxes)   update_hitbox.y  = this->y;
        }
    }
}


void Unit::tick() {
    if (is_player) {
        move(init_move());
    } else {

    }
}


void Unit::become_player() {
    is_player = true;
}

void Unit::loose_player() {
    is_player = false;
    control_resistance = -1;
}