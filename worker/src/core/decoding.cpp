#include "decoding.hpp"

std::string huffmanDecoding(const CompressedData &data) {
  if (data.originalSize == 0)
    return "";

  Node *root = buildHuffmanTree(data.freqTable);
  std::string decoded = "";

  // Single character edge case
  if (!root->left && !root->right) {
    decoded = std::string(data.originalSize, root->ch);
    deleteTree(root);
    return decoded;
  }

  Node *curr = root;
  uint32_t charsDecoded = 0;
  uint32_t bitsProcessed = 0;

  for (uint8_t byte : data.packedBytes) {
    for (int i = 7; i >= 0; --i) {
      if (charsDecoded >= data.originalSize ||
          bitsProcessed >= data.totalBitCount)
        break;

      // Extract bit at position i (from MSB to LSB)
      bool bit = (byte >> i) & 1;
      curr = bit ? curr->right : curr->left;

      if (!curr->left && !curr->right) {
        decoded += curr->ch;
        charsDecoded++;
        curr = root;
      }
      bitsProcessed++;
    }
  }

  deleteTree(root);
  return decoded;
}
