#include "core/decoding.hpp"
#include "core/encoding.hpp"
#include "core/stats.hpp"
#include <iostream>

int main() {
  std::string input = "This is a long string intended to demonstrate real "
                      "bit-level Huffman compression.";

  CompressedData compressed = huffmanEncoding(input);
  CompressionStats stats = getStats(compressed);

  std::cout << "Original Size: " << stats.originalBytes << " bytes"
            << std::endl;
  std::cout << "Compressed Size (including metadata): " << stats.compressedBytes
            << " bytes" << std::endl;
  std::cout << "Space Saving: " << stats.spaceSaving << "%" << std::endl;

  std::string decoded = huffmanDecoding(compressed);
  if (decoded == "Thisisalongstringintendedtodemonstraterealbit-"
                 "levelHuffmancompression.") {
    std::cout << "Decoded successfully!" << std::endl;
  }

  return 0;
}
