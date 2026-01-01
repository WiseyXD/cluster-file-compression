#include "./core/decoding.hpp" // Include your new decoding file
#include "./core/encoding.hpp"
#include <iostream>
#include <map>

int main() {
  std::string input = "pranav";
  std::cout << "Original: " << input << std::endl;

  // 1. Calculate frequencies (the metadata)
  // In a real app, you might make huffmanEncoding return this map
  std::map<char, int> freq;
  for (char c : input) {
    if (!std::isspace(c)) {
      freq[c]++;
    }
  }

  // 2. Encode
  // Note: If you changed your function signature to return a pair,
  // adjust this line accordingly.
  std::string encoded = huffmanEncoding(input);
  std::cout << "Encoded : " << encoded << std::endl;

  // 3. Decode
  // The decoder uses the bitstream AND the frequency map to rebuild the tree
  std::string decoded = huffmanDecoding(encoded, freq);
  std::cout << "Decoded : " << decoded << std::endl;

  // 4. Validation
  if (decoded == "pranav") {
    std::cout << "SUCCESS: Round-trip verified!" << std::endl;
  } else {
    std::cout << "FAILURE: Decoded text does not match." << std::endl;
  }

  // 5. Test Edge Case: Single Character
  std::string edge_input = "aaaaa";
  std::map<char, int> edge_freq;
  for (char c : edge_input)
    edge_freq[c]++;

  std::string edge_encoded = huffmanEncoding(edge_input);
  std::string edge_decoded = huffmanDecoding(edge_encoded, edge_freq);

  std::cout << "\nEdge Case 'aaaaa':" << std::endl;
  std::cout << "Encoded: " << edge_encoded << std::endl;
  std::cout << "Decoded: " << edge_decoded << std::endl;

  return 0;
}
