#include "decoding.hpp"
#include <map>
#include <string>

std::string huffmanDecoding(std::string encoded_message,
                            std::map<char, int> freq) {
  if (encoded_message.empty() || freq.empty()) {
    return "";
  }

  // 1. Rebuild the exact same tree
  Node *root = buildHuffmanTree(freq);
  std::string decoded_output = "";
  Node *current = root;

  // 2. Special case: Single unique character
  if (!root->left && !root->right) {
    // If there is only one character, the encoded_message is just '0' repeated
    // freq[root->ch] tells us how many times to repeat it
    return std::string(freq[root->ch], root->ch);
  }

  // 3. Traverse bits
  for (char bit : encoded_message) {
    if (bit == '0') {
      current = current->left;
    } else {
      current = current->right;
    }

    // If leaf node found
    if (!current->left && !current->right) {
      decoded_output += current->ch;
      current = root; // Go back to start
    }
  }

  deleteTree(root);
  return decoded_output;
}
