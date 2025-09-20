#include <string>
#include "helper_utils.h"


std::string replaceSubstring(const std::string& original, const std::string& toReplace, const std::string& replaceWith) {
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
