#include "encoding.hpp"
#include "huffman_tree.hpp"
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

std::string huffmanEncoding(std::string s) {
  s.erase(std::remove_if(s.begin(), s.end(),
                         [](unsigned char c) { return std::isspace(c); }),
          s.end());

  if (s.empty())
    return "";

  std::map<char, int> freq;
  for (char c : s)
    freq[c]++;

  Node *root = buildHuffmanTree(freq);

  std::map<char, std::string> codes;
  generateCode(root, "", codes);

  std::string encoded_message = "";
  for (char c : s) {
    encoded_message += codes[c];
  }

  deleteTree(root);
  return encoded_message;
}
