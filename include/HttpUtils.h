#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H

#include <string>
#include <curl/curl.h>

// Configuration constants
const int DEFAULT_TIMEOUT_SECONDS = 30;
const int MAX_RETRIES = 3;
const int INITIAL_BACKOFF_MS = 1000;
const int MAX_BACKOFF_MS = 10000;

// HTTP status codes
enum class HttpStatus {
    OK = 200,
    CREATED = 201,
    NO_CONTENT = 204,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500
};

/**
 * @brief Logs an informational message with timestamp
 * @param message Message to log
 */
void log_info(const std::string& message);

/**
 * @brief Logs an error message with timestamp
 * @param message Error message to log
 */
void log_error(const std::string& message);

/**
 * @brief Logs a warning message with timestamp
 * @param message Warning message to log
 */
void log_warning(const std::string& message);

/**
 * @brief Implements exponential backoff with jitter
 * @param attempt Current attempt number (0-based)
 */
void exponential_backoff(int attempt);

/**
 * @brief Checks if an HTTP status code indicates a retryable error
 * @param status_code HTTP status code
 * @return true if the error is retryable, false otherwise
 */
bool is_retryable_error(int status_code);

/**
 * @brief Checks if a cURL error is retryable
 * @param code cURL error code
 * @return true if the error is retryable, false otherwise
 */
bool is_retryable_curl_error(CURLcode code);

/**
 * @brief cURL write callback function
 * @param contents Pointer to received data
 * @param size Size of each data element
 * @param nmemb Number of data elements
 * @param userp User pointer (string buffer)
 * @return Number of bytes processed
 */
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

#endif // HTTP_UTILS_H 