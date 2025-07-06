#include<iostream>
#include<curl/curl.h>
#include<string>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

void perform_get(const char* url) {
  CURL* curl = curl_easy_init();
  if(curl) {
    std::string response_string;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    CURLcode res = curl_easy_perform(curl);
    if(res == CURLE_OK) {
      long response_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      std::cout << "GET response code: " << response_code << std::endl;
      std::cout << "GET response body: " << response_string << std::endl;
    }
    curl_easy_cleanup(curl);
  }
}

void perform_post(const char* url, const char* data) {
  CURL* curl = curl_easy_init();
  if(curl) {
    std::string response_string;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    CURLcode res = curl_easy_perform(curl);
    if(res == CURLE_OK) {
      long response_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      std::cout << "POST response code: " << response_code << std::endl;
      std::cout << "POST response body: " << response_string << std::endl;
    }
    curl_easy_cleanup(curl);
  }
}

void perform_put(const char* url, const char* data) {
  CURL* curl = curl_easy_init();
  if(curl) {
    std::string response_string;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    CURLcode res = curl_easy_perform(curl);
    if(res == CURLE_OK) {
      long response_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      std::cout << "PUT response code: " << response_code << std::endl;
      std::cout << "PUT response body: " << response_string << std::endl;
    }
    curl_easy_cleanup(curl);
  }
}

void perform_delete(const char* url) {
  CURL* curl = curl_easy_init();
  if(curl) {
    std::string response_string;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    CURLcode res = curl_easy_perform(curl);
    if(res == CURLE_OK) {
      long response_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      std::cout << "DELETE response code: " << response_code << std::endl;
      std::cout << "DELETE response body: " << response_string << std::endl;
    }
    curl_easy_cleanup(curl);
  }
}

int main(int argc, char *argv[]) {
  std::cout << "Hello World!" << std::endl;

  const char* get_url = "https://httpbin.org/get";
  const char* post_url = "https://httpbin.org/post";
  const char* put_url = "https://httpbin.org/put";
  const char* delete_url = "https://httpbin.org/delete";
  const char* data = "foo=bar";

  perform_get(get_url);
  perform_post(post_url, data);
  perform_put(put_url, data);
  perform_delete(delete_url);

  return 0;
}
