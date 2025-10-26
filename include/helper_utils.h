#ifndef HELPER_UTILS_H
#define HELPER_UTILS_H

#include <string>
#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>

// String ops //
std::string replace_substring(const std::string& original, const std::string& toReplace, const std::string& replaceWith);
std::vector<std::string> split(const std::string& str, char delimiter);

// File Ops //
std::string strip_comments(const std::string& input);
nlohmann::json get_json(std::string path_json_file);

// misc //
struct XY {
    int y = 0;
    int x = 0;
};

struct Hitbox {
    int type; // 0: none; 1: collides with walls & stuff; 2: takes damage from attacks
    int x, y; // Stuck to the units x & y coords
    int x_offset, y_offset;
    int width, height;

    bool colliding(const Hitbox& B) {
        int A_full_x = x + x_offset;
        int A_full_y = y + y_offset;
        int B_full_x = B.x + B.x_offset;
        int B_full_y = B.y + B.y_offset;
        return !(A_full_x + width < B_full_x  || A_full_x > B_full_x + B.width || 
                A_full_y + height < B_full_y || A_full_y > B_full_y + B.height
                );
    }
};


#endif