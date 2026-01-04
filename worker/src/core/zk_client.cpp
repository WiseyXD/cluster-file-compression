#include "zk_client.hpp"
#include <stdexcept>
#include <unistd.h>

void global_watcher(zhandle_t *, int, int, const char *, void *ctx) {
  auto *self = static_cast<ZkClient *>(ctx);
  std::unique_lock<std::mutex> lk(self->mtx);
  self->cv.notify_all();
}

ZkClient::ZkClient(const std::string &hosts) {
  zh = zookeeper_init(hosts.c_str(), global_watcher, 30000, nullptr, this, 0);
  if (!zh)
    throw std::runtime_error("ZooKeeper init failed");
}

ZkClient::~ZkClient() {
  if (zh)
    zookeeper_close(zh);
}

void ZkClient::createPersistent(const std::string &path,
                                const std::string &data) {
  zoo_create(zh, path.c_str(), data.data(), data.size(), &ZOO_OPEN_ACL_UNSAFE,
             0, nullptr, 0);
}

std::string ZkClient::createEphemeralSequential(const std::string &path,
                                                const std::string &data) {
  char buffer[512];
  zoo_create(zh, path.c_str(), data.data(), data.size(), &ZOO_OPEN_ACL_UNSAFE,
             ZOO_EPHEMERAL | ZOO_SEQUENCE, buffer, sizeof(buffer));
  return buffer;
}

std::string ZkClient::get(const std::string &path) {
  char buffer[4096];
  int len = sizeof(buffer);
  zoo_get(zh, path.c_str(), 0, buffer, &len, nullptr);
  return std::string(buffer, len);
}

std::vector<std::string> ZkClient::getChildren(const std::string &path) {
  String_vector children;
  zoo_get_children(zh, path.c_str(), 0, &children);
  std::vector<std::string> result(children.count);
  for (int i = 0; i < children.count; i++)
    result[i] = children.data[i];
  deallocate_String_vector(&children);
  return result;
}

void ZkClient::waitForChildren(const std::string &path, int expected) {
  while (true) {
    String_vector children;
    zoo_get_children(zh, path.c_str(), 1, &children);
    if (children.count >= expected) {
      deallocate_String_vector(&children);
      return;
    }
    deallocate_String_vector(&children);
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk);
  }
}
