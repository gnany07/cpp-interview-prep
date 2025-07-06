#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <vector>
#include <curl/curl.h>
#include "ApiException.h"

/**
 * @brief HTTP Response structure containing response data and metadata
 */
struct HttpResponse {
    int status_code;           ///< HTTP status code
    std::string body;          ///< Response body
    std::string error_message; ///< Error message if request failed
    bool success;              ///< Whether the request was successful
    
    HttpResponse() : status_code(0), success(false) {}
};

/**
 * @brief Robust HTTP client with retry logic, timeout handling, and error management
 * 
 * This class provides a production-ready HTTP client that handles:
 * - Automatic retries with exponential backoff
 * - Timeout management
 * - SSL verification
 * - Comprehensive error handling
 * - Resource cleanup
 */
class HttpClient {
private:
    CURL* curl_;                    ///< cURL handle
    int timeout_seconds_;           ///< Request timeout in seconds
    
    /**
     * @brief Sets up common cURL options for all requests
     */
    void setup_common_options();
    
public:
    /**
     * @brief Constructs an HttpClient with specified timeout
     * @param timeout_seconds Request timeout in seconds (default: 30)
     * @throws std::runtime_error if cURL initialization fails
     */
    HttpClient(int timeout_seconds = 30);
    
    /**
     * @brief Destructor - cleans up cURL handle
     */
    ~HttpClient();
    
    /**
     * @brief Makes an HTTP request with retry logic and error handling
     * @param url Target URL
     * @param method HTTP method (GET, POST, PUT, DELETE)
     * @param data Request body data (for POST/PUT)
     * @param headers HTTP headers to include
     * @return HttpResponse containing response data and status
     */
    HttpResponse make_request(const std::string& url, 
                             const std::string& method = "GET",
                             const std::string& data = "",
                             const std::vector<std::string>& headers = {});
    
    // Disable copy constructor and assignment operator
    HttpClient(const HttpClient&) = delete;
    HttpClient& operator=(const HttpClient&) = delete;
};

#endif // HTTP_CLIENT_H 