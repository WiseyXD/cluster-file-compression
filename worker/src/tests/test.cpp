#include "../core/decoding.hpp"
#include "../core/encoding.hpp"
#include "../core/types.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string>

TEST_CASE("Huffman Encoding/Decoding Round Trip", "[huffman]") {

  SECTION("Standard string: pranav") {
    std::string input = "pranav";

    // The new encoder returns a CompressedData object
    CompressedData compressed = huffmanEncoding(input);

    // The new decoder takes that object directly
    std::string decoded = huffmanDecoding(compressed);

    REQUIRE(decoded == "pranav");
    // Verify metadata was captured
    REQUIRE(compressed.freqTable.at('p') == 1);
    REQUIRE(compressed.freqTable.at('a') == 2);
  }

  SECTION("Single character: aaaaa") {
    std::string input = "aaaaa";
    CompressedData compressed = huffmanEncoding(input);
    std::string decoded = huffmanDecoding(compressed);

    REQUIRE(decoded == "aaaaa");
    // In a single-char case, bit count should ideally be 5 (if encoded as '0's)
    REQUIRE(compressed.totalBitCount == 5);
  }

  SECTION("Empty string") {
    CompressedData compressed = huffmanEncoding("");
    REQUIRE(compressed.totalBitCount == 0);
    REQUIRE(huffmanDecoding(compressed) == "");
  }
}

TEST_CASE("Data Integrity", "[huffman]") {
  SECTION("Magic bytes and versioning") {
    CompressedData compressed = huffmanEncoding("test");
    REQUIRE(compressed.magic[0] == 'H');
    REQUIRE(compressed.magic[1] == 'F');
    REQUIRE(compressed.version == 1);
  }
}

TEST_CASE("Basic Math", "[smoke_test]") { REQUIRE(2 + 2 == 4); }
