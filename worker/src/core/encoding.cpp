#include "encoding.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <queue>
#include <utility>
#include <vector>

using namespace std;

struct Node {
  char ch;
  int freq;
  Node *left;
  Node *right;
  Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};
struct Compare {
  bool operator()(Node *a, Node *b) { return a->freq > b->freq; };
};

void generateCode(Node *root, string path, unordered_map<char, string> &codes) {
  if (!root || root == nullptr) {
    return;
  }
  if (!root->right && !root->left) {
    codes[root->ch] = path.empty() ? "0" : path;
    return;
  }

  generateCode(root->left, path + "0", codes);
  generateCode(root->right, path + "1", codes);
};

void deleteTree(Node *node) {
  if (node == nullptr) {
    return;
  }
  deleteTree(node->left);
  deleteTree(node->right);
  delete node; // Delete the current node after its children
}

string huffmanEncoding(string s) {

  std::unordered_map<char, int> freq;
  priority_queue<Node *, vector<Node *>, Compare> min_heap;

  // remove whitespaces
  s.erase(std::remove_if(s.begin(), s.end(),
                         [](unsigned char c) { return std::isspace(c); }),
          s.end());

  cout << s << endl;

  // count the frequency in an pair of array or map
  for (char &c : s) {
    freq[c]++;
  }
  for (auto &it : freq) {
    min_heap.push(new Node(it.first, it.second));
  }

  while (min_heap.size() > 1) {
    Node *left = min_heap.top();
    min_heap.pop();

    Node *right = min_heap.top();
    min_heap.pop();

    Node *parent = new Node('\0', right->freq + left->freq);
    parent->left = left;
    parent->right = right;
    min_heap.push(parent);
  }

  Node *root = min_heap.top();

  // generate codes
  unordered_map<char, string> codes;
  generateCode(root, "", codes);

  // encoded string
  string encoded_message;
  for (char c : s) { // Iterate through the original input string
    cout << c << " : " << codes[c] << endl;
    encoded_message += codes[c];
  }

  deleteTree(root);
  return encoded_message;
}
