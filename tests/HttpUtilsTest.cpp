#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "HttpUtils.h"
#include <curl/curl.h>
#include <sstream>
#include <chrono>
#include <thread>

class HttpUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Redirect cout/cerr to capture log output
        old_cout = std::cout.rdbuf();
        old_cerr = std::cerr.rdbuf();
        std::cout.rdbuf(cout_buffer.rdbuf());
        std::cerr.rdbuf(cerr_buffer.rdbuf());
    }

    void TearDown() override {
        // Restore cout/cerr
        std::cout.rdbuf(old_cout);
        std::cerr.rdbuf(old_cerr);
    }

    std::stringstream cout_buffer;
    std::stringstream cerr_buffer;
    std::streambuf* old_cout;
    std::streambuf* old_cerr;
};

// Test logging functions
TEST_F(HttpUtilsTest, LogInfoTest) {
    std::string test_message = "Test info message";
    log_info(test_message);
    
    std::string output = cout_buffer.str();
    EXPECT_THAT(output, ::testing::HasSubstr("INFO: " + test_message));
    EXPECT_THAT(output, ::testing::HasSubstr("INFO:"));
}

TEST_F(HttpUtilsTest, LogErrorTest) {
    std::string test_message = "Test error message";
    log_error(test_message);
    
    std::string output = cerr_buffer.str();
    EXPECT_THAT(output, ::testing::HasSubstr("ERROR: " + test_message));
    EXPECT_THAT(output, ::testing::HasSubstr("ERROR:"));
}

TEST_F(HttpUtilsTest, LogWarningTest) {
    std::string test_message = "Test warning message";
    log_warning(test_message);
    
    std::string output = cout_buffer.str();
    EXPECT_THAT(output, ::testing::HasSubstr("WARNING: " + test_message));
    EXPECT_THAT(output, ::testing::HasSubstr("WARNING:"));
}

// Test exponential backoff
TEST_F(HttpUtilsTest, ExponentialBackoffTest) {
    auto start = std::chrono::high_resolution_clock::now();
    
    exponential_backoff(1); // Should wait ~1000ms with jitter
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should wait at least 500ms (0.5 * 1000ms)
    EXPECT_GE(duration.count(), 500);
    // Should wait at most 1500ms (1.5 * 1000ms)
    EXPECT_LE(duration.count(), 1500);
}

TEST_F(HttpUtilsTest, ExponentialBackoffZeroAttempt) {
    auto start = std::chrono::high_resolution_clock::now();
    
    exponential_backoff(0); // Should not wait
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should be very fast (no wait)
    EXPECT_LT(duration.count(), 100);
}

// Test retryable error checking
TEST_F(HttpUtilsTest, RetryableErrorTest) {
    // Test 5xx errors (should be retryable)
    EXPECT_TRUE(is_retryable_error(500));
    EXPECT_TRUE(is_retryable_error(502));
    EXPECT_TRUE(is_retryable_error(503));
    EXPECT_TRUE(is_retryable_error(504));
    
    // Test rate limiting (should be retryable)
    EXPECT_TRUE(is_retryable_error(429));
    
    // Test 4xx errors (should not be retryable)
    EXPECT_FALSE(is_retryable_error(400));
    EXPECT_FALSE(is_retryable_error(401));
    EXPECT_FALSE(is_retryable_error(404));
    
    // Test 2xx/3xx (should not be retryable)
    EXPECT_FALSE(is_retryable_error(200));
    EXPECT_FALSE(is_retryable_error(201));
    EXPECT_FALSE(is_retryable_error(302));
}

TEST_F(HttpUtilsTest, RetryableCurlErrorTest) {
    // Test retryable cURL errors
    EXPECT_TRUE(is_retryable_curl_error(CURLE_COULDNT_CONNECT));
    EXPECT_TRUE(is_retryable_curl_error(CURLE_COULDNT_RESOLVE_HOST));
    EXPECT_TRUE(is_retryable_curl_error(CURLE_OPERATION_TIMEDOUT));
    EXPECT_TRUE(is_retryable_curl_error(CURLE_SSL_CONNECT_ERROR));
    EXPECT_TRUE(is_retryable_curl_error(CURLE_GOT_NOTHING));
    EXPECT_TRUE(is_retryable_curl_error(CURLE_SEND_ERROR));
    EXPECT_TRUE(is_retryable_curl_error(CURLE_RECV_ERROR));
    
    // Test non-retryable cURL errors
    EXPECT_FALSE(is_retryable_curl_error(CURLE_OK));
    EXPECT_FALSE(is_retryable_curl_error(CURLE_UNSUPPORTED_PROTOCOL));
    EXPECT_FALSE(is_retryable_curl_error(CURLE_URL_MALFORMAT));
}

// Test WriteCallback function
TEST_F(HttpUtilsTest, WriteCallbackTest) {
    std::string test_data = "Hello, World!";
    std::string buffer;
    
    size_t result = WriteCallback(
        const_cast<char*>(test_data.c_str()),
        1,  // size
        test_data.length(),  // nmemb
        &buffer
    );
    
    EXPECT_EQ(result, test_data.length());
    EXPECT_EQ(buffer, test_data);
}

TEST_F(HttpUtilsTest, WriteCallbackEmptyData) {
    std::string buffer = "existing data";
    std::string original_buffer = buffer;
    
    size_t result = WriteCallback(
        nullptr,
        0,  // size
        0,  // nmemb
        &buffer
    );
    
    EXPECT_EQ(result, 0);
    EXPECT_EQ(buffer, original_buffer); // Should not change
}

// Test configuration constants
TEST_F(HttpUtilsTest, ConfigurationConstantsTest) {
    EXPECT_EQ(DEFAULT_TIMEOUT_SECONDS, 30);
    EXPECT_EQ(MAX_RETRIES, 3);
    EXPECT_EQ(INITIAL_BACKOFF_MS, 1000);
    EXPECT_EQ(MAX_BACKOFF_MS, 10000);
}

// Test HTTP status codes enum
TEST_F(HttpUtilsTest, HttpStatusEnumTest) {
    EXPECT_EQ(static_cast<int>(HttpStatus::OK), 200);
    EXPECT_EQ(static_cast<int>(HttpStatus::CREATED), 201);
    EXPECT_EQ(static_cast<int>(HttpStatus::NO_CONTENT), 204);
    EXPECT_EQ(static_cast<int>(HttpStatus::BAD_REQUEST), 400);
    EXPECT_EQ(static_cast<int>(HttpStatus::UNAUTHORIZED), 401);
    EXPECT_EQ(static_cast<int>(HttpStatus::NOT_FOUND), 404);
    EXPECT_EQ(static_cast<int>(HttpStatus::INTERNAL_SERVER_ERROR), 500);
} 