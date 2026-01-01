#include "stats.hpp"
#include <filesystem>

CompressionStats getStatsFromFile(const std::string &filename,
                                  const CompressedData &data) {
  CompressionStats stats;

  stats.originalBytes = data.originalSize;
  stats.compressedBytes = std::filesystem::file_size(filename);

  stats.ratio =
      static_cast<double>(stats.compressedBytes) / stats.originalBytes;
  stats.spaceSaving = (1.0 - stats.ratio) * 100.0;

  return stats;
}
