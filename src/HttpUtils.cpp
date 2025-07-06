#include "HttpUtils.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <ctime>

// Utility functions
void log_info(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::cout << "[" << std::ctime(&time_t) << "] INFO: " << message << std::endl;
}

void log_error(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::cerr << "[" << std::ctime(&time_t) << "] ERROR: " << message << std::endl;
}

void log_warning(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::cout << "[" << std::ctime(&time_t) << "] WARNING: " << message << std::endl;
}

// Exponential backoff with jitter
void exponential_backoff(int attempt) {
    if (attempt == 0) return;
    
    int backoff_ms = std::min(INITIAL_BACKOFF_MS * (1 << (attempt - 1)), MAX_BACKOFF_MS);
    
    // Add jitter (random factor between 0.5 and 1.5)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.5, 1.5);
    backoff_ms = static_cast<int>(backoff_ms * dis(gen));
    
    log_info("Retrying in " + std::to_string(backoff_ms) + "ms (attempt " + std::to_string(attempt + 1) + ")");
    std::this_thread::sleep_for(std::chrono::milliseconds(backoff_ms));
}

// Check if status code indicates retryable error
bool is_retryable_error(int status_code) {
    return status_code >= 500 || status_code == 429; // 5xx errors or rate limit
}

// Check if cURL error is retryable
bool is_retryable_curl_error(CURLcode code) {
    switch (code) {
        case CURLE_COULDNT_CONNECT:
        case CURLE_COULDNT_RESOLVE_HOST:
        case CURLE_OPERATION_TIMEDOUT:
        case CURLE_SSL_CONNECT_ERROR:
        case CURLE_GOT_NOTHING:
        case CURLE_SEND_ERROR:
        case CURLE_RECV_ERROR:
            return true;
        default:
            return false;
    }
}

// cURL write callback function
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
} 