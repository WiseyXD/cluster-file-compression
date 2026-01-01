#pragma once
#include <cstdint>
#include <map>
#include <vector>

struct CompressedData {
  char magic[2] = {'H', 'F'};
  uint8_t version = 1;

  std::map<char, int> freqTable;
  std::vector<uint8_t> packedBytes;

  uint32_t originalSize = 0;  // Stopping rule 1
  uint32_t totalBitCount = 0; // Stopping rule 2
};

struct CompressionStats {
  size_t originalBytes;
  size_t compressedBytes;
  double ratio;
  double spaceSaving;
};
