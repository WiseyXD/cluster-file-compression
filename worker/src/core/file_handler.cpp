#include "file_handler.hpp"
#include <fstream>

bool saveToFile(const std::string &filename, const CompressedData &data) {
  std::ofstream os(filename, std::ios::binary);
  if (!os)
    return false;

  // Header
  os.write(data.magic, 2);
  os.write(reinterpret_cast<const char *>(&data.version), 1);

  // Mandatory for decoding
  os.write(reinterpret_cast<const char *>(&data.originalSize), 4);

  // Frequency table
  uint32_t mapSize = data.freqTable.size();
  os.write(reinterpret_cast<const char *>(&mapSize), 4);
  for (const auto &[ch, freq] : data.freqTable) {
    os.put(ch);
    os.write(reinterpret_cast<const char *>(&freq), 4);
  }

  // Bitstream metadata + payload
  os.write(reinterpret_cast<const char *>(&data.totalBitCount), 4);

  uint32_t dataSize = data.packedBytes.size();
  os.write(reinterpret_cast<const char *>(&dataSize), 4);
  os.write(reinterpret_cast<const char *>(data.packedBytes.data()), dataSize);

  return true;
}

bool loadFromFile(const std::string &filename, CompressedData &data) {
  std::ifstream is(filename, std::ios::binary);
  if (!is)
    return false;

  is.read(data.magic, 2);
  is.read(reinterpret_cast<char *>(&data.version), 1);

  // Required for correct decoding
  is.read(reinterpret_cast<char *>(&data.originalSize), 4);

  uint32_t mapSize;
  is.read(reinterpret_cast<char *>(&mapSize), 4);
  data.freqTable.clear();

  for (uint32_t i = 0; i < mapSize; ++i) {
    char ch = is.get();
    int freq;
    is.read(reinterpret_cast<char *>(&freq), 4);
    data.freqTable[ch] = freq;
  }

  is.read(reinterpret_cast<char *>(&data.totalBitCount), 4);

  uint32_t dataSize;
  is.read(reinterpret_cast<char *>(&dataSize), 4);
  data.packedBytes.resize(dataSize);
  is.read(reinterpret_cast<char *>(data.packedBytes.data()), dataSize);

  return true;
}
