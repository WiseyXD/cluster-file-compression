#include "bit_packer.hpp"

std::vector<uint8_t> packBits(const std::string &bitString) {
  std::vector<uint8_t> packed;
  uint8_t currentByte = 0;
  int bitCount = 0;

  for (char bit : bitString) {
    if (bit == '1') {
      currentByte |= (1 << (7 - bitCount));
    }
    bitCount++;
    if (bitCount == 8) {
      packed.push_back(currentByte);
      currentByte = 0;
      bitCount = 0;
    }
  }
  if (bitCount > 0)
    packed.push_back(currentByte);
  return packed;
}
