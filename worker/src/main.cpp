#include "core/decoding.hpp"
#include "core/encoding.hpp"
#include "core/file_handler.hpp"
#include "core/stats.hpp"
#include <iostream>

int main() {
  std::string input = "This is a long string intended to demonstrate real "
                      "bit-level Huffman compression.";

  // 1. Encode
  CompressedData compressed = huffmanEncoding(input);

  // 2. Write to file
  if (!saveToFile("out.huff", compressed)) {
    std::cerr << "Failed to write compressed file\n";
    return 1;
  }

  // 3. Stats must come from the real file
  CompressionStats stats = getStatsFromFile("out.huff", compressed);

  std::cout << "Original Size: " << stats.originalBytes << " bytes\n";
  std::cout << "Compressed Size (including metadata): " << stats.compressedBytes
            << " bytes\n";
  std::cout << "Space Saving: " << stats.spaceSaving << "%\n";

  // 4. Read back from file
  CompressedData loaded;
  if (!loadFromFile("out.huff", loaded)) {
    std::cerr << "Failed to read compressed file\n";
    return 1;
  }

  // 5. Decode
  std::string decoded = huffmanDecoding(loaded);

  if (decoded == input) {
    std::cout << "Decoded successfully!\n";
  } else {
    std::cerr << "Decoded output mismatch!\n";
  }

  return 0;
}
