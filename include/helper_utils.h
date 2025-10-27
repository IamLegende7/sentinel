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
    int x = 0;
    int y = 0;
};

class Hitbox {
    public:
        int type; // 0: none; 1: collides with walls & stuff; 2: takes damage from attacks
        int x, y; // Stuck to the units x & y coords
        int x_offset, y_offset;
        int width, height;

        bool colliding(const Hitbox& B);
        void render(SDL_Renderer* renderer, bool player = false);
};


#endif