#include <map>

struct Node {
  char ch;
  int freq;
  Node *left;
  Node *right;
  Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

// Common tree building logic
Node *buildHuffmanTree(const std::map<char, int> &freq);

void deleteTree(Node *node);
