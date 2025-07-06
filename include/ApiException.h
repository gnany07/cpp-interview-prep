#ifndef API_EXCEPTION_H
#define API_EXCEPTION_H

#include <stdexcept>
#include <string>

/**
 * @brief Custom exception class for API-related errors
 * 
 * This exception class extends std::runtime_error and adds
 * HTTP status code tracking for better error handling.
 */
class ApiException : public std::runtime_error {
public:
    /**
     * @brief Constructs an ApiException with a message and optional status code
     * @param message Error message
     * @param status_code HTTP status code (default: 0)
     */
    ApiException(const std::string& message, int status_code = 0) 
        : std::runtime_error(message), status_code_(status_code) {}
    
    /**
     * @brief Gets the HTTP status code associated with this exception
     * @return HTTP status code
     */
    int get_status_code() const { return status_code_; }
    
private:
    int status_code_;
};

#endif // API_EXCEPTION_H 