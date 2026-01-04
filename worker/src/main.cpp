#include "core/decoding.hpp"
#include "core/encoding.hpp"
#include "core/zk_client.hpp"
#include "generated/huffman.pb.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

constexpr int NUM_WORKERS = 2;

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

  std::string input = "This is a long string intended to demonstrate real "
                      "bit-level Huffman compression.";

  /* -------- Step 1: local frequency count -------- */

  std::map<char, int> localFreq = countFrequencies(input);

  FrequencyTable protoFreq = toProto(localFreq);
  std::string serialized;
  protoFreq.SerializeToString(&serialized);

  zk.createPersistent("/huffman/frequencies/" + workerId, serialized);

  /* -------- Step 2: barrier -------- */

  zk.createEphemeralSequential("/huffman/barrier/worker-", "");
  zk.waitForChildren("/huffman/barrier", NUM_WORKERS);

  /* -------- Step 3: leader merges -------- */

  auto workers = zk.getChildren("/huffman/workers");
  std::sort(workers.begin(), workers.end());

  if (workerId == workers[0]) {
    auto globalFreq = mergeFrequencies(zk, workers);

    FrequencyTable globalProto = toProto(globalFreq);
    globalProto.SerializeToString(&serialized);

    zk.createPersistent("/huffman/global_freq", serialized);
  }

  /* -------- Step 4: all workers read global freq -------- */

  auto data = zk.get("/huffman/global_freq");
  FrequencyTable globalProto;
  globalProto.ParseFromString(data);

  std::map<char, int> globalFreq = fromProto(globalProto);

  /* -------- Step 5: encode with existing Huffman -------- */

  CompressedData compressed = huffmanEncoding(input, globalFreq);

  // (send compressed chunk to API service or file)
}
