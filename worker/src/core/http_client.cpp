#include "http_client.hpp"
#include <curl/curl.h>
#include <iostream>

bool postData(const std::string &url, const std::vector<uint8_t> &data,
              const std::string &workerId) {

  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Failed to init curl\n";
    return false;
  }

  struct curl_slist *headers = nullptr;
  std::string workerHeader = "X-Worker-ID: " + workerId;
  headers = curl_slist_append(headers, workerHeader.c_str());
  headers =
      curl_slist_append(headers, "Content-Type: application/octet-stream");

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.data());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(data.size()));

  // Connection timeout (10s) and operation timeout (30s)
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

  CURLcode res = curl_easy_perform(curl);

  long httpCode = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  if (res != CURLE_OK) {
    std::cerr << "curl error: " << curl_easy_strerror(res) << "\n";
    return false;
  }

  if (httpCode != 200) {
    std::cerr << "HTTP error: " << httpCode << "\n";
    return false;
  }

  std::cout << "Successfully posted data to API (worker: " << workerId << ")\n";
  return true;
}
