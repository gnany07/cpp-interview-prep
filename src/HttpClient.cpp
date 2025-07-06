#include "HttpClient.h"
#include "HttpUtils.h"
#include <curl/curl.h>
#include <stdexcept>
#include <algorithm>

HttpClient::HttpClient(int timeout_seconds) 
    : timeout_seconds_(timeout_seconds) {
    curl_ = curl_easy_init();
    if (!curl_) {
        throw std::runtime_error("Failed to initialize cURL");
    }
    setup_common_options();
}

HttpClient::~HttpClient() {
    if (curl_) {
        curl_easy_cleanup(curl_);
    }
}

void HttpClient::setup_common_options() {
    curl_easy_setopt(curl_, CURLOPT_TIMEOUT, timeout_seconds_);
    curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_, CURLOPT_MAXREDIRS, 3L);
    curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl_, CURLOPT_USERAGENT, "C++-API-Client/1.0");
}

HttpResponse HttpClient::make_request(const std::string& url, 
                                     const std::string& method,
                                     const std::string& data,
                                     const std::vector<std::string>& headers) {
    
    HttpResponse response;
    
    for (int attempt = 0; attempt <= MAX_RETRIES; ++attempt) {
        try {
            log_info("Making " + method + " request to " + url + " (attempt " + std::to_string(attempt + 1) + ")");
            
            // Reset cURL options
            curl_easy_reset(curl_);
            setup_common_options();
            
            // Set URL
            curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
            
            // Set method
            if (method == "POST") {
                curl_easy_setopt(curl_, CURLOPT_POST, 1L);
            } else if (method == "PUT") {
                curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "PUT");
            } else if (method == "DELETE") {
                curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "DELETE");
            }
            
            // Set data if provided
            if (!data.empty()) {
                curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data.c_str());
            }
            
            // Set headers
            struct curl_slist* header_list = nullptr;
            for (const auto& header : headers) {
                header_list = curl_slist_append(header_list, header.c_str());
            }
            if (header_list) {
                curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header_list);
            }
            
            // Set callback
            curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteCallback);
            curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response.body);
            
            // Perform request
            CURLcode res = curl_easy_perform(curl_);
            
            // Get status code
            long http_code = 0;
            curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code);
            response.status_code = static_cast<int>(http_code);
            
            // Clean up headers
            if (header_list) {
                curl_slist_free_all(header_list);
            }
            
            // Check for cURL errors
            if (res != CURLE_OK) {
                response.error_message = curl_easy_strerror(res);
                log_error("cURL error: " + response.error_message);
                
                if (is_retryable_curl_error(res) && attempt < MAX_RETRIES) {
                    exponential_backoff(attempt);
                    continue;
                } else {
                    throw ApiException("cURL error: " + response.error_message);
                }
            }
            
            // Check HTTP status code
            if (response.status_code >= 200 && response.status_code < 300) {
                response.success = true;
                log_info("Request successful with status code: " + std::to_string(response.status_code));
                return response;
            } else {
                response.error_message = "HTTP " + std::to_string(response.status_code);
                log_warning("HTTP error: " + response.error_message);
                
                if (is_retryable_error(response.status_code) && attempt < MAX_RETRIES) {
                    exponential_backoff(attempt);
                    continue;
                } else {
                    throw ApiException("HTTP error: " + std::to_string(response.status_code), response.status_code);
                }
            }
            
        } catch (const std::exception& e) {
            log_error("Request failed: " + std::string(e.what()));
            if (attempt == MAX_RETRIES) {
                response.error_message = e.what();
                return response;
            }
            exponential_backoff(attempt);
        }
    }
    
    return response;
} 