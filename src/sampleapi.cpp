#include <iostream>
#include <curl/curl.h>
#include <string>
#include <nlohmann/json.hpp>
#include "HttpClient.h"
#include "HttpUtils.h"

using json = nlohmann::json;

// API endpoints
const char* BASE_URL = "https://jsonplaceholder.typicode.com";
const char* POSTS_ENDPOINT = "/posts";

// API functions using the separated HttpClient class

void perform_get() {
    try {
        HttpClient client;
        std::string get_url = std::string(BASE_URL) + POSTS_ENDPOINT + "/1";
        
        HttpResponse response = client.make_request(get_url, "GET");
        
        if (response.success) {
            try {
                // Parse JSON response
                json json_response = json::parse(response.body);
                
                std::cout << "GET Response Parsed:" << std::endl;
                std::cout << "  ID: " << json_response["id"] << std::endl;
                std::cout << "  Title: " << json_response["title"] << std::endl;
                std::cout << "  Body: " << json_response["body"] << std::endl;
                std::cout << "  User ID: " << json_response["userId"] << std::endl;
                std::cout << std::endl;
            } catch (const json::parse_error& e) {
                log_error("Error parsing JSON: " + std::string(e.what()));
                std::cout << "Raw response: " << response.body << std::endl;
            }
        } else {
            log_error("GET request failed: " + response.error_message);
        }
    } catch (const std::exception& e) {
        log_error("GET request exception: " + std::string(e.what()));
    }
}

void perform_post() {
    try {
        HttpClient client;
        std::string post_url = std::string(BASE_URL) + POSTS_ENDPOINT;
        
        // Create JSON object
        json post_data;
        post_data["title"] = "foo";
        post_data["body"] = "bar";
        post_data["userId"] = 1;
        
        // Convert JSON to string
        std::string data = post_data.dump();
        
        // Set headers
        std::vector<std::string> headers = {"Content-Type: application/json; charset=UTF-8"};
        
        HttpResponse response = client.make_request(post_url, "POST", data, headers);
        
        if (response.success) {
            try {
                // Parse JSON response
                json json_response = json::parse(response.body);
                
                std::cout << "POST Response Parsed:" << std::endl;
                std::cout << "  ID: " << json_response["id"] << std::endl;
                std::cout << "  Title: " << json_response["title"] << std::endl;
                std::cout << "  Body: " << json_response["body"] << std::endl;
                std::cout << "  User ID: " << json_response["userId"] << std::endl;
                std::cout << std::endl;
            } catch (const json::parse_error& e) {
                log_error("Error parsing JSON: " + std::string(e.what()));
                std::cout << "Raw response: " << response.body << std::endl;
            }
        } else {
            log_error("POST request failed: " + response.error_message);
        }
    } catch (const std::exception& e) {
        log_error("POST request exception: " + std::string(e.what()));
    }
}

void perform_put() {
    try {
        HttpClient client;
        std::string put_url = std::string(BASE_URL) + POSTS_ENDPOINT + "/1";
        
        // Create JSON object
        json put_data;
        put_data["id"] = 1;
        put_data["title"] = "foo";
        put_data["body"] = "bar";
        put_data["userId"] = 1;
        
        // Convert JSON to string
        std::string data = put_data.dump();
        
        // Set headers
        std::vector<std::string> headers = {"Content-Type: application/json; charset=UTF-8"};
        
        HttpResponse response = client.make_request(put_url, "PUT", data, headers);
        
        if (response.success) {
            try {
                // Parse JSON response
                json json_response = json::parse(response.body);
                
                std::cout << "PUT Response Parsed:" << std::endl;
                std::cout << "  ID: " << json_response["id"] << std::endl;
                std::cout << "  Title: " << json_response["title"] << std::endl;
                std::cout << "  Body: " << json_response["body"] << std::endl;
                std::cout << "  User ID: " << json_response["userId"] << std::endl;
                std::cout << std::endl;
            } catch (const json::parse_error& e) {
                log_error("Error parsing JSON: " + std::string(e.what()));
                std::cout << "Raw response: " << response.body << std::endl;
            }
        } else {
            log_error("PUT request failed: " + response.error_message);
        }
    } catch (const std::exception& e) {
        log_error("PUT request exception: " + std::string(e.what()));
    }
}

void perform_delete() {
    try {
        HttpClient client;
        std::string delete_url = std::string(BASE_URL) + POSTS_ENDPOINT + "/1";
        
        HttpResponse response = client.make_request(delete_url, "DELETE");
        
        if (response.success) {
            try {
                // Parse JSON response
                json json_response = json::parse(response.body);
                
                std::cout << "DELETE Response Parsed:" << std::endl;
                std::cout << "  Response: " << json_response.dump(2) << std::endl;
                std::cout << std::endl;
            } catch (const json::parse_error& e) {
                log_error("Error parsing JSON: " + std::string(e.what()));
                std::cout << "Raw response: " << response.body << std::endl;
            }
        } else {
            log_error("DELETE request failed: " + response.error_message);
        }
    } catch (const std::exception& e) {
        log_error("DELETE request exception: " + std::string(e.what()));
    }
}

int main(int argc, char *argv[]) {
    try {
        log_info("Starting Sample API Integration with Best Practices");
        
        // Initialize cURL globally
        CURLcode init_result = curl_global_init(CURL_GLOBAL_DEFAULT);
        if (init_result != CURLE_OK) {
            log_error("Failed to initialize cURL: " + std::string(curl_easy_strerror(init_result)));
            return 1;
        }
        
        log_info("cURL initialized successfully");
        
        // Perform API operations with proper error handling
        try {
            perform_get();
        } catch (const std::exception& e) {
            log_error("GET operation failed: " + std::string(e.what()));
        }
        
        try {
            perform_post();
        } catch (const std::exception& e) {
            log_error("POST operation failed: " + std::string(e.what()));
        }
        
        try {
            perform_put();
        } catch (const std::exception& e) {
            log_error("PUT operation failed: " + std::string(e.what()));
        }
        
        try {
            perform_delete();
        } catch (const std::exception& e) {
            log_error("DELETE operation failed: " + std::string(e.what()));
        }
        
        log_info("All API operations completed");
        
    } catch (const std::exception& e) {
        log_error("Fatal error: " + std::string(e.what()));
        return 1;
    }
    
    // Cleanup
    curl_global_cleanup();
    log_info("cURL cleanup completed");
    
    return 0;
}
