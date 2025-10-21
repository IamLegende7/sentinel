#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>

#include "helper_utils.h"
#include "main.h"


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
    return json_data;
}
