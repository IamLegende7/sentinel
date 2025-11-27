#ifndef INFO_H
#define INFO_H

#include <string>

struct Version {
    int major;
    int minor;
    int patch;
    std::string build;

    std::string toString() const {
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch) + "-" + build;
    }
};

const std::string INFO_NAMESPACE = "sentinel";
const std::string INFO_DOMAIN = "de.thealloy.sentinel";
const std::string INFO_URL = "https://github.com/IamLegende7/sentinel";
const std::string INFO_NAME = "The Sentinel";
const std::string INFO_AUTHOR = "Legende_7";
const std::string INFO_COPYRIGHT = "(c) Legende_7 2025";
const Version INFO_VERSION = {0, 0, 0, "ALPHA"};

#endif