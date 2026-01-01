#include "stats.hpp"

CompressionStats getStats(const CompressedData &data) {
  CompressionStats stats;
  stats.originalBytes = data.originalSize;

  // Calculate total metadata size + packed data size
  size_t metadataSize = sizeof(data.magic) + sizeof(data.version) +
                        sizeof(uint32_t) * 2; // originalSize + bitCount
  metadataSize += data.freqTable.size() * (sizeof(char) + sizeof(int));

  stats.compressedBytes = metadataSize + data.packedBytes.size();

  stats.ratio = (double)stats.compressedBytes / stats.originalBytes;
  stats.spaceSaving = (1.0 - stats.ratio) * 100.0;

  return stats;
}
