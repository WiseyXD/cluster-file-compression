
#include "huffman_tree.hpp"
#include <algorithm>
#include <map>
#include <queue>
#include <vector>

struct Compare {
  bool operator()(Node *a, Node *b) {
    if (a->freq == b->freq) {
      return a->ch > b->ch; // Tie-breaker
    }
    return a->freq > b->freq;
  }
};

// Common tree building logic
Node *buildHuffmanTree(const std::map<char, int> &freq) {
  if (freq.empty())
    return nullptr;

  std::priority_queue<Node *, std::vector<Node *>, Compare> min_heap;
  for (auto const &[ch, f] : freq) {
    min_heap.push(new Node(ch, f));
  }

  // Handle single unique character case:
  // The while loop won't run, leaving one node in the heap.
  while (min_heap.size() > 1) {
    Node *left = min_heap.top();
    min_heap.pop();
    Node *right = min_heap.top();
    min_heap.pop();

    // Internal node gets the smaller char as a representative for tie-breaking
    Node *parent =
        new Node(std::min(left->ch, right->ch), left->freq + right->freq);
    parent->left = left;
    parent->right = right;
    min_heap.push(parent);
  }
  return min_heap.top();
}

void deleteTree(Node *node) {
  if (!node)
    return;
  deleteTree(node->left);
  deleteTree(node->right);
  delete node;
}
