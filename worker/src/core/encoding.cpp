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

std::map<char, int> countFrequencies(const std::string &input) {
  std::map<char, int> freq;
  for (char c : input)
    freq[c]++;
  return freq;
}

CompressedData huffmanEncoding(const std::string &input,
                               const std::map<char, int> &globalFreq) {

  Node *root = buildHuffmanTree(globalFreq);

  std::map<char, std::string> codes;
  generateCode(root, "", codes);

  // 1. Build bit string
  std::string bitString;
  for (char c : input) {
    bitString += codes[c];
  }

  // 2. Pack bits
  std::vector<uint8_t> packed = packBits(bitString);

  // 3. Fill CompressedData
  CompressedData out;
  out.freqTable = globalFreq;
  out.packedBytes = packed;
  out.originalSize = input.size();
  out.totalBitCount = bitString.size();

  deleteTree(root);
  return out;
}

CompressedData huffmanEncoding(std::string s) {
  CompressedData result;
  // Standard cleaning as per your earlier requirements
  trim(s);

  if (s.empty())
    return result;

  result.originalSize = s.length();
  for (char c : s)
    result.freqTable[c]++;

  Node *root = buildHuffmanTree(result.freqTable);
  std::map<char, std::string> codes;
  // Assuming generateCode is accessible here
  generateCode(root, "", codes);

  // Bit Packing Logic
  uint8_t currentByte = 0;
  int bitPos = 0;

  for (char c : s) {
    const std::string &code = codes[c];
    for (char bit : code) {
      // Shift current bits left and add new bit at LSB
      currentByte <<= 1;
      if (bit == '1')
        currentByte |= 1;
      bitPos++;

      if (bitPos == 8) {
        result.packedBytes.push_back(currentByte);
        currentByte = 0;
        bitPos = 0;
      }
      result.totalBitCount++;
    }
  }

  // Flush remaining bits in the partial last byte
  if (bitPos > 0) {
    currentByte <<= (8 - bitPos); // Align bits to MSB
    result.packedBytes.push_back(currentByte);
  }

  deleteTree(root);
  return result;
}
