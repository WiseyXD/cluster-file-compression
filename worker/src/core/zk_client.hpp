#pragma once
#include <condition_variable>
#include <mutex>
#include <string>
#include <vector>
#include <zookeeper/zookeeper.h>

class ZkClient;

// 👇 forward-declared watcher
void global_watcher(zhandle_t *, int, int, const char *, void *);

class ZkClient {
public:
  explicit ZkClient(const std::string &hosts);
  ~ZkClient();

  std::string createEphemeralSequential(const std::string &path,
                                        const std::string &data);

  void createPersistent(const std::string &path, const std::string &data = "");

  std::string get(const std::string &path);
  std::vector<std::string> getChildren(const std::string &path);
  void waitForChildren(const std::string &path, int expected);

private:
  friend void global_watcher(zhandle_t *, int, int, const char *, void *);

  zhandle_t *zh;
  std::mutex mtx;
  std::condition_variable cv;
};
