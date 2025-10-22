#ifndef HELPER_UTILS_H
#define HELPER_UTILS_H

#include <string>
#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>

std::string replace_substring(const std::string& original, const std::string& toReplace, const std::string& replaceWith);

// File Ops //
std::string strip_comments(const std::string& input);
nlohmann::json get_json(std::string path_json_file);

// misc
struct XY {
    int y = 0;
    int x = 0;
};

#endif