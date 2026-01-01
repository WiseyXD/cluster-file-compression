#pragma once

#include <cstdint>
#include <map>
#include <vector>

struct CompressedData {
  char magic[2] = {'H', 'F'};
  uint8_t version = 1;
  std::map<char, int> freqTable;
  std::vector<uint8_t> packedBytes;
  uint32_t totalBitCount = 0; // Crucial for ignoring padding in the last byte
};
