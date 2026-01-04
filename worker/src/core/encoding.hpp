#pragma once

#include "types.hpp"
#include <string>
#include <unordered_map>

using namespace std;

struct Node;
struct Compare;

void generateCode(Node *root, string path, unordered_map<char, string> &codes);

std::map<char, int> countFrequencies(const std::string &input);

void deleteTree(Node *node);

CompressedData huffmanEncoding(const std::string &input,
                               const std::map<char, int> &globalFreq);

CompressedData huffmanEncoding(std::string s);
