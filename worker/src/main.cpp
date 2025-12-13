#include <algorithm>
#include <functional>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;
vector<pair<char, int>> frequencyTable;

std::unordered_map<char, int> freq;

string huffmanEncoding(string s) {
  // remove whitespaces
  // count the frequency in an pair of array or map

  // noob worker
  priority_queue<int, vector<int>, greater<int>> min_heap;

  s.erase(std::remove_if(s.begin(), s.end(),
                         [](unsigned char c) { return std::isspace(c); }),
          s.end());

  for (char c : s) {
    freq[c]++;
  }

  return s;
}

int main() {
  string ex = "binomial coeffecient";
  string ans = huffmanEncoding(ex);
  for (const auto p : freq) {
    std::cout << p.first << " : " << p.second << "\n";
  }
  return 0;
}
