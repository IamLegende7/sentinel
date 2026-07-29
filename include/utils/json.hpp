#ifndef JSON_HPP
#define JSON_HPP

#include <fstream>
#include <sstream>
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/filereadstream.h"

// Printing out
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>

#include "utils/logger.hpp"
#include "settings/debug.hpp"

std::string open_file(const std::string& filename);
rapidjson::Document open_json(const std::string filename);

#endif