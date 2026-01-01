#pragma once
#include "types.hpp"
#include <string>

CompressionStats getStatsFromFile(const std::string &filename,
                                  const CompressedData &data);
