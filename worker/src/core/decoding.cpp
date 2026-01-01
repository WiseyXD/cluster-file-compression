#include "decoding.hpp"

std::string huffmanDecoding(const CompressedData &data) {
  if (data.totalBitCount == 0 || data.freqTable.empty())
    return "";

  Node *root = buildHuffmanTree(data.freqTable);
  std::string decoded = "";

  // --- CRITICAL FIX FOR SINGLE CHARACTER CASE ---
  if (!root->left && !root->right) {
    // There is only one unique character.
    // We know how many times it repeats from the frequency table.
    decoded = std::string(data.freqTable.begin()->second, root->ch);
    deleteTree(root);
    return decoded;
  }
  // ----------------------------------------------

  Node *curr = root;
  uint32_t processedBits = 0;
  for (uint8_t byte : data.packedBytes) {
    for (int i = 0; i < 8; ++i) {
      if (processedBits >= data.totalBitCount)
        break;

      bool bit = (byte >> (7 - i)) & 1;
      curr = bit ? curr->right : curr->left;

      if (!curr->left && !curr->right) {
        decoded += curr->ch;
        curr = root;
      }
      processedBits++;
    }
  }

  deleteTree(root);
  return decoded;
}
