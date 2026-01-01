#pragma once

#include "types.hpp"
#include <string>
#include <unordered_map>

using namespace std;

struct Node;
struct Compare;

void generateCode(Node *root, string path, unordered_map<char, string> &codes);

void deleteTree(Node *node);

CompressedData huffmanEncoding(std::string s);
