#pragma once
#include "types.hpp"
#include <string>

bool saveToFile(const std::string &filename, const CompressedData &data);
bool loadFromFile(const std::string &filename, CompressedData &data);
