#include "./core/decoding.hpp" // Include your new decoding file
#include "./core/encoding.hpp"
#include "./core/file_handler.hpp"
#include "./core/types.hpp"
#include <iostream>

int main() {
  std::string input = "huffman compression works";

  // Encode
  CompressedData compressed = huffmanEncoding(input);
  saveToFile("compressed.huff", compressed);

  // Load & Decode
  CompressedData loaded;
  loadFromFile("compressed.huff", loaded);
  std::string output = huffmanDecoding(loaded);

  std::cout << "Original: " << input << "\nDecoded: " << output << std::endl;
  return 0;
}
