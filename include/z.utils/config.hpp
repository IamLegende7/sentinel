#ifndef CONFIG_HPP
#define CONFIG_HPP


// general
#include <string>
#include <variant>

#include "logger.hpp"

// loading / saving settings
#include <fstream>
#include <sstream>
#include <vector>
#include <type_traits>

// for storing settings
#include <unordered_map>

// Setting struct //
template <typename T>
struct Setting {
    std::string name;
    std::string file;
    T value;

    Setting() : name(""), file("data/config/general.ini"), value(T()) {}

    Setting(const std::string& name, const std::string file = "data/config/general.ini") : name(name), file(file) {
        load_value();
    }

    T get() {
        return value;
    }

    void set(T value) {
        this->value = value;
    }

    void load_value() {
        std::ifstream config_file(file);
        std::string line;

        if (config_file.is_open()) {
            while (getline(config_file, line)) {
                std::istringstream iss(line);
                std::string key, equal, loaded_value;

                if (std::getline(iss, key, '=') && std::getline(iss, loaded_value)) {
                    if (key == name) {
                        try {
                            load_value_impl(loaded_value);
                        } catch (...) {
                            LOGGER.log(LogLevel::ERROR, "[utils/config.hpp:load_value] Could not load setting %s", name.c_str());
                        }
                        break;
                    }
                }
            }
            config_file.close();
        } else {
            LOGGER.log(LogLevel::ERROR, "[utils/config.hpp:load_value] Unable to open file: %s", file.c_str());
        }
    }

    private:
        // bool //
        template<typename U = T>
        typename std::enable_if<std::is_same<U, bool>::value>::type
        load_value_impl(const std::string& loaded_value) {
            value = (loaded_value == "true");
        }

        // int //
        template<typename U = T>
        typename std::enable_if<std::is_same<U, int>::value>::type
        load_value_impl(const std::string& loaded_value) {
            value = std::stoi(loaded_value);
        }

        // float //
        template<typename U = T>
        typename std::enable_if<std::is_same<U, float>::value>::type
        load_value_impl(const std::string& loaded_value) {
            value = std::stof(loaded_value);
        }

        // std:.string //
        template<typename U = T>
        typename std::enable_if<std::is_same<U, std::string>::value>::type
        load_value_impl(const std::string& loaded_value) {
            value = loaded_value;
        }

        // unknown //
        template<typename U = T>
        typename std::enable_if<!std::is_same<U, bool>::value && 
                                !std::is_same<U, int>::value && 
                                !std::is_same<U, float>::value && 
                                !std::is_same<U, std::string>::value>::type
        load_value_impl(const std::string& loaded_value) {
            LOGGER.log(LogLevel::ERROR, "[utils/config.hpp:load_value] Unknown type");
        }
};

using SettingVariant = std::variant<Setting<bool>, Setting<int>, Setting<float>, Setting<std::string>>;

// WARNING: FAILS QUIETLY
template <typename T>
T getvariant(const SettingVariant& setting_variant) {
    if (std::holds_alternative<Setting<T>>(setting_variant)) {
        return std::get<Setting<T>>(setting_variant).get();
    }
}

#endif