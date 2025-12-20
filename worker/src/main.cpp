#include "./core/encoding.hpp"
#include <iostream>

int main() {
  std::string ex = "pranav";
  std::string ans = huffmanEncoding(ex);
  std::cout << "Encoded :" << ans << std::endl;
  return 0;
}
