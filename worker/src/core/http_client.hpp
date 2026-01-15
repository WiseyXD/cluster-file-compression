#pragma once
#include <cstdint>
#include <string>
#include <vector>

// POST binary data to the given URL with X-Worker-ID header
bool postData(const std::string &url, const std::vector<uint8_t> &data,
              const std::string &workerId);
