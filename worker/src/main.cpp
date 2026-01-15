#include "core/decoding.hpp"
#include "core/encoding.hpp"
#include "core/http_client.hpp"
#include "core/zk_client.hpp"
#include "generated/huffman.pb.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

constexpr int NUM_WORKERS = 2;
constexpr const char *INPUT_FILE = "/data/input.txt";
constexpr const char *API_URL = "http://huffman-api:8080/submit";

/* ---------------- File input helper ---------------- */

std::string readInputFile(const std::string &path) {
  std::ifstream file(path);
  if (!file) {
    std::cerr << "Warning: Could not open " << path
              << ", using fallback input\n";
    return "This is a long string intended to demonstrate real bit-level "
           "Huffman compression.";
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

/* ---------------- Serialize CompressedData to binary ---------------- */

std::vector<uint8_t> serializeCompressedData(const CompressedData &data) {
  std::vector<uint8_t> result;

  // Magic bytes
  result.push_back(static_cast<uint8_t>(data.magic[0]));
  result.push_back(static_cast<uint8_t>(data.magic[1]));

  // Version
  result.push_back(data.version);

  // Original size (4 bytes, little-endian)
  result.push_back(data.originalSize & 0xFF);
  result.push_back((data.originalSize >> 8) & 0xFF);
  result.push_back((data.originalSize >> 16) & 0xFF);
  result.push_back((data.originalSize >> 24) & 0xFF);

  // Frequency table size (4 bytes)
  uint32_t mapSize = data.freqTable.size();
  result.push_back(mapSize & 0xFF);
  result.push_back((mapSize >> 8) & 0xFF);
  result.push_back((mapSize >> 16) & 0xFF);
  result.push_back((mapSize >> 24) & 0xFF);

  // Frequency table entries
  for (const auto &[ch, freq] : data.freqTable) {
    result.push_back(static_cast<uint8_t>(ch));
    result.push_back(freq & 0xFF);
    result.push_back((freq >> 8) & 0xFF);
    result.push_back((freq >> 16) & 0xFF);
    result.push_back((freq >> 24) & 0xFF);
  }

  // Total bit count (4 bytes)
  result.push_back(data.totalBitCount & 0xFF);
  result.push_back((data.totalBitCount >> 8) & 0xFF);
  result.push_back((data.totalBitCount >> 16) & 0xFF);
  result.push_back((data.totalBitCount >> 24) & 0xFF);

  // Packed bytes size (4 bytes)
  uint32_t dataSize = data.packedBytes.size();
  result.push_back(dataSize & 0xFF);
  result.push_back((dataSize >> 8) & 0xFF);
  result.push_back((dataSize >> 16) & 0xFF);
  result.push_back((dataSize >> 24) & 0xFF);

  // Packed bytes
  result.insert(result.end(), data.packedBytes.begin(), data.packedBytes.end());

  return result;
}

/* ---------------- Protobuf helpers ---------------- */

FrequencyTable toProto(const std::map<char, int> &freq) {
  FrequencyTable table;
  for (auto &[c, n] : freq) {
    auto *e = table.add_entries();
    e->set_symbol(static_cast<int>(c));
    e->set_count(n);
  }
  return table;
}

std::map<char, int> fromProto(const FrequencyTable &table) {
  std::map<char, int> freq;
  for (auto &e : table.entries()) {
    freq[(char)e.symbol()] += e.count();
  }
  return freq;
}

/* ------------- Merge all worker frequencies ------------- */

std::map<char, int> mergeFrequencies(ZkClient &zk,
                                     const std::vector<std::string> &workers) {

  std::map<char, int> merged;

  for (auto &w : workers) {
    auto data = zk.get("/huffman/frequencies/" + w);
    FrequencyTable table;
    table.ParseFromString(data);

    auto local = fromProto(table);
    for (auto &[c, n] : local)
      merged[c] += n;
  }

  return merged;
}

/* --------------------------- MAIN --------------------------- */

int main() {
  std::cout << "=== Huffman Worker Starting ===\n";

  ZkClient zk("zookeeper:2181");

  // Ensure base znodes exist (idempotent)
  zk.createPersistent("/huffman");
  zk.createPersistent("/huffman/workers");
  zk.createPersistent("/huffman/frequencies");
  zk.createPersistent("/huffman/barrier");
  zk.createPersistent("/huffman/global_freq");

  // Register worker
  auto workerPath =
      zk.createEphemeralSequential("/huffman/workers/worker-", "");
  auto workerId = workerPath.substr(workerPath.find_last_of('/') + 1);

  std::cout << "Worker registered: " << workerId << "\n";

  // Read input from file (or use fallback)
  std::string input = readInputFile(INPUT_FILE);
  std::cout << "Input size: " << input.size() << " bytes\n";

  /* -------- Step 1: local frequency count -------- */

  std::map<char, int> localFreq = countFrequencies(input);

  FrequencyTable protoFreq = toProto(localFreq);
  std::string serialized;
  protoFreq.SerializeToString(&serialized);

  zk.createPersistent("/huffman/frequencies/" + workerId, serialized);
  std::cout << "Frequency table published\n";

  /* -------- Step 2: barrier -------- */

  zk.createEphemeralSequential("/huffman/barrier/worker-", "");
  std::cout << "Waiting for " << NUM_WORKERS << " workers at barrier...\n";
  zk.waitForChildren("/huffman/barrier", NUM_WORKERS);
  std::cout << "Barrier passed\n";

  /* -------- Step 3: leader merges -------- */

  auto workers = zk.getChildren("/huffman/workers");
  std::sort(workers.begin(), workers.end());

  if (workerId == workers[0]) {
    std::cout << "I am the leader, merging frequencies...\n";
    auto globalFreq = mergeFrequencies(zk, workers);

    FrequencyTable globalProto = toProto(globalFreq);
    globalProto.SerializeToString(&serialized);

    zk.set("/huffman/global_freq", serialized);
    std::cout << "Global frequency table published\n";
  }

  /* -------- Step 3.5: barrier to wait for leader -------- */
  zk.createPersistent("/huffman/barrier2");
  zk.createEphemeralSequential("/huffman/barrier2/ready-", "");
  std::cout << "Waiting for all workers at barrier2...\n";
  zk.waitForChildren("/huffman/barrier2", NUM_WORKERS);
  std::cout << "Barrier2 passed\n";

  /* -------- Step 4: all workers read global freq -------- */

  auto data = zk.get("/huffman/global_freq");
  FrequencyTable globalProto;
  globalProto.ParseFromString(data);

  std::map<char, int> globalFreq = fromProto(globalProto);
  std::cout << "Read global frequency table (" << globalFreq.size()
            << " symbols)\n";

  /* -------- Step 5: encode with shared Huffman tree -------- */

  CompressedData compressed = huffmanEncoding(input, globalFreq);
  std::cout << "Compression complete: " << compressed.packedBytes.size()
            << " bytes (from " << compressed.originalSize << ")\n";

  /* -------- Step 5.5: VERIFY by decoding -------- */

  std::string decoded = huffmanDecoding(compressed);
  bool verified = (decoded == input);
  std::cout << "Decode verification: " << (verified ? "PASSED ✓" : "FAILED ✗")
            << "\n";
  if (!verified) {
    std::cerr << "ERROR: Decoded output does not match input!\n";
    std::cerr << "  Input length:   " << input.size() << "\n";
    std::cerr << "  Decoded length: " << decoded.size() << "\n";
  } else {
    std::cout << "  Original:  \"" << input.substr(0, 50) << "...\"\n";
    std::cout << "  Decoded:   \"" << decoded.substr(0, 50) << "...\"\n";
  }

  /* -------- Step 6: serialize and POST to API -------- */

  std::vector<uint8_t> binaryData = serializeCompressedData(compressed);
  std::cout << "Serialized data size: " << binaryData.size() << " bytes\n";

  if (postData(API_URL, binaryData, workerId)) {
    std::cout << "=== Worker " << workerId << " completed successfully ===\n";
  } else {
    std::cerr << "=== Worker " << workerId << " failed to post data ===\n";
    return 1;
  }

  return 0;
}
