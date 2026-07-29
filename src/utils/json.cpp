#include "utils/json.hpp"

std::string open_file(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;

    if (file) {
        buffer << file.rdbuf();
    } else {
        LOGGER.log(LogLevel::WARNING, "Could not open file: %s", filename.c_str());
        return "{}";
    }

    return buffer.str();
}

rapidjson::Document open_json(const std::string filename) { // TODO: Add more debug!
    rapidjson::Document document;
    try {
        std::string json_contents = open_file(filename);

        // Parse
        document.Parse<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag>(json_contents.c_str(), strlen(json_contents.c_str()));

        // Check errors
        if (document.HasParseError()) {
            int error_offset = document.GetErrorOffset();
            const char* error_message = rapidjson::GetParseError_En(document.GetParseError());
            LOGGER.log(LogLevel::WARNING, "Error parsing JSON %s: %s (offset: %d)", filename.c_str(), error_message, error_offset);
            document = rapidjson::Document();
            document.SetObject();
        }

    } catch (const std::exception& e) {
        LOGGER.log(LogLevel::WARNING, "Could not load json %s: %s", filename.c_str(), e.what());
        document = rapidjson::Document();
        document.SetObject();
    }

    if (DEBUG["annoying_logs"]) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);

        LOGGER.log(LogLevel::DEBUG, "Loaded Json %s:", filename.c_str());
        std::cout << buffer.GetString() << std::endl;
    }


    return document;
}