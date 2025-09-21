#ifndef HELPER_UTILS_H
#define HELPER_UTILS_H

#include <string>

std::string replaceSubstring(const std::string& original, const std::string& toReplace, const std::string& replaceWith);
struct XY {
    int y = 0;
    int x = 0;
};

#endif