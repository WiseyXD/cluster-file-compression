#include "encoding.hpp"
#include "bit_packer.hpp"
#include "huffman_tree.hpp"
#include "types.hpp"
#include <algorithm>
#include <map>

void generateCode(Node *root, std::string path,
                  std::map<char, std::string> &codes) {
  if (!root)
    return;
  if (!root->left && !root->right) {
    codes[root->ch] = path.empty() ? "0" : path;
    return;
  }
  generateCode(root->left, path + "0", codes);
  generateCode(root->right, path + "1", codes);
}

void trim(std::string &s) {
  // 1. Trim from the end (Right trim)
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());

  // 2. Trim from the start (Left trim)
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
}

CompressedData huffmanEncoding(std::string s) {
  CompressedData result;

  trim(s);

  if (s.empty())
    return result;

  for (char c : s)
    result.freqTable[c]++;

  Node *root = buildHuffmanTree(result.freqTable);
  std::map<char, std::string> codes;
  generateCode(root, "", codes);

  std::string bitString = "";
  for (char c : s)
    bitString += codes[c];

  result.totalBitCount = bitString.length();
  result.packedBytes = packBits(bitString);

  deleteTree(root);
  return result;
}
