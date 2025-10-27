#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>

#include "helper_utils.h"
#include "main.h"
#include "logger.hpp"


std::string replace_substring(const std::string& original, const std::string& toReplace, const std::string& replaceWith) {
    std::string result = original;
    size_t pos = 0;

    // Find the position of the substring to replace
    while ((pos = result.find(toReplace, pos)) != std::string::npos) {
        // Replace the substring
        result.replace(pos, toReplace.length(), replaceWith);
        // Move past the replaced substring
        pos += replaceWith.length();
    }

    return result;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    size_t start = 0, end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos) {
        token = str.substr(start, end - start);
        tokens.push_back(token);
        start = end + 1;
    }
    tokens.push_back(str.substr(start));
    return tokens;
}

std::string strip_comments(const std::string& input) {
    std::istringstream iss(input);
    std::string output;
    std::string line;

    while (std::getline(iss, line)) {
        size_t comment_pos = line.find("//");
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        output += line + "\n";
    }
    return output;
}

nlohmann::json get_json(std::string path_json_file) {
    std::ifstream json_file(path_json_file);
    if (!json_file.is_open()) {
        LOGGER.log(LogLevel::ERROR, "Failed to open json file: %s", path_json_file);
        return nlohmann::json{};
    }

    std::ostringstream buffer;
    buffer << json_file.rdbuf();
    json_file.close();
    std::string json_string = buffer.str();

    std::string cleaned_json = strip_comments(json_string);
    nlohmann::json json_data = nlohmann::json{};

    try {
        json_data = nlohmann::json::parse(cleaned_json);
    } catch (const std::exception& e) {}

    if ( json_data.is_null() ) {
        LOGGER.log(LogLevel::ERROR, "Could not load json file %s", path_json_file);
        json_data = nlohmann::json{};
    }
    return json_data;
}

//////////////
// HITBOXES //
//////////////

bool Hitbox::colliding(const Hitbox& B) {
    int A_full_x = x; //+ x_offset;
    int A_full_y = y; //+ y_offset;
    int B_full_x = B.x + B.x_offset;
    int B_full_y = B.y + B.y_offset;
    return !(A_full_x + width  < B_full_x || A_full_x > B_full_x + B.width ||
             A_full_y + height < B_full_y || A_full_y > B_full_y + B.height
            );
}

void Hitbox::render(SDL_Renderer* renderer, bool player) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);  // set coulor to pink/magenta/whatever
    SDL_FRect hitbox_rect = {};
    if (player) {
        hitbox_rect = {(float)(RENDER_OFFSET_X + x_offset), (float)(RENDER_OFFSET_Y + y_offset), (float)width, (float)height};
    } else {
        hitbox_rect = {(float)(RENDER_OFFSET_X - CAMERA_POS.x + x_offset), (float)(RENDER_OFFSET_Y - CAMERA_POS.y + y_offset), (float)width, (float)height};
    }
    SDL_RenderRect(renderer, &hitbox_rect);
}