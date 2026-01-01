#include "../core/decoding.hpp"
#include "../core/encoding.hpp"
#include <catch2/catch_test_macros.hpp>
#include <map>

TEST_CASE("Huffman Encoding/Decoding Round Trip", "[huffman]") {
  std::string input = "pranav";

  // Setup metadata
  std::map<char, int> freq;
  for (char c : input)
    freq[c]++;

  SECTION("Standard string") {
    std::string encoded = huffmanEncoding(input);
    std::string decoded = huffmanDecoding(encoded, freq);
    REQUIRE(decoded == "pranav");
  }

  SECTION("Single character") {
    std::string s = "aaaaa";
    std::map<char, int> f = {{'a', 5}};
    std::string encoded = huffmanEncoding(s);
    REQUIRE(huffmanDecoding(encoded, f) == "aaaaa");
  }

  SECTION("Empty string") { REQUIRE(huffmanEncoding("") == ""); }
}

TEST_CASE("basic math") { REQUIRE(2 + 2 == 4); };
