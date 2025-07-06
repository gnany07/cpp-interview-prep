#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "HttpClient.h"
#include "HttpUtils.h"
#include <curl/curl.h>
#include <stdexcept>

class HttpClientTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize cURL globally for tests
        curl_global_init(CURL_GLOBAL_DEFAULT);
        
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
        
        // Cleanup cURL
        curl_global_cleanup();
    }

    std::stringstream cout_buffer;
    std::stringstream cerr_buffer;
    std::streambuf* old_cout;
    std::streambuf* old_cerr;
};

// Test HttpClient constructor
TEST_F(HttpClientTest, ConstructorTest) {
    EXPECT_NO_THROW({
        HttpClient client(30);
    });
}

TEST_F(HttpClientTest, ConstructorDefaultTimeout) {
    EXPECT_NO_THROW({
        HttpClient client; // Should use default timeout
    });
}

// Test HttpClient destructor
TEST_F(HttpClientTest, DestructorTest) {
    HttpClient* client = new HttpClient();
    EXPECT_NO_THROW({
        delete client; // Should clean up cURL handle
    });
}

// Test successful GET request
TEST_F(HttpClientTest, SuccessfulGetRequest) {
    HttpClient client(5); // Short timeout for testing
    
    HttpResponse response = client.make_request(
        "https://httpbin.org/get",
        "GET"
    );
    
    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.status_code, 200);
    EXPECT_FALSE(response.body.empty());
    EXPECT_TRUE(response.error_message.empty());
}

// Test successful POST request
TEST_F(HttpClientTest, SuccessfulPostRequest) {
    HttpClient client(5);
    
    std::string json_data = "{\"test\": \"data\"}";
    std::vector<std::string> headers = {"Content-Type: application/json"};
    
    HttpResponse response = client.make_request(
        "https://httpbin.org/post",
        "POST",
        json_data,
        headers
    );
    
    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.status_code, 200);
    EXPECT_FALSE(response.body.empty());
    EXPECT_TRUE(response.error_message.empty());
}

// Test successful PUT request
TEST_F(HttpClientTest, SuccessfulPutRequest) {
    HttpClient client(5);
    
    std::string json_data = "{\"test\": \"updated\"}";
    std::vector<std::string> headers = {"Content-Type: application/json"};
    
    HttpResponse response = client.make_request(
        "https://httpbin.org/put",
        "PUT",
        json_data,
        headers
    );
    
    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.status_code, 200);
    EXPECT_FALSE(response.body.empty());
    EXPECT_TRUE(response.error_message.empty());
}

// Test successful DELETE request
TEST_F(HttpClientTest, SuccessfulDeleteRequest) {
    HttpClient client(5);
    
    HttpResponse response = client.make_request(
        "https://httpbin.org/delete",
        "DELETE"
    );
    
    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.status_code, 200);
    EXPECT_FALSE(response.body.empty());
    EXPECT_TRUE(response.error_message.empty());
}

// Test request with invalid URL
TEST_F(HttpClientTest, InvalidUrlTest) {
    HttpClient client(5);
    
    HttpResponse response = client.make_request(
        "https://invalid-domain-that-does-not-exist-12345.com",
        "GET"
    );
    
    EXPECT_FALSE(response.success);
    EXPECT_FALSE(response.error_message.empty());
}

// Test request with 404 error
TEST_F(HttpClientTest, NotFoundErrorTest) {
    HttpClient client(5);
    
    HttpResponse response = client.make_request(
        "https://httpbin.org/status/404",
        "GET"
    );
    
    EXPECT_FALSE(response.success);
    EXPECT_EQ(response.status_code, 404);
    EXPECT_FALSE(response.error_message.empty());
}

// Test request with 500 error
TEST_F(HttpClientTest, ServerErrorTest) {
    HttpClient client(5);
    
    HttpResponse response = client.make_request(
        "https://httpbin.org/status/500",
        "GET"
    );
    
    EXPECT_FALSE(response.success);
    EXPECT_EQ(response.status_code, 500);
    EXPECT_FALSE(response.error_message.empty());
}

// Test request with timeout
TEST_F(HttpClientTest, TimeoutTest) {
    HttpClient client(1); // Very short timeout
    
    HttpResponse response = client.make_request(
        "https://httpbin.org/delay/3", // 3 second delay
        "GET"
    );
    
    EXPECT_FALSE(response.success);
    EXPECT_FALSE(response.error_message.empty());
}

// Test request with custom headers
TEST_F(HttpClientTest, CustomHeadersTest) {
    HttpClient client(5);
    
    std::vector<std::string> headers = {
        "Content-Type: application/json",
        "Authorization: Bearer test-token",
        "X-Custom-Header: test-value"
    };
    
    HttpResponse response = client.make_request(
        "https://httpbin.org/headers",
        "GET",
        "",
        headers
    );
    
    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.status_code, 200);
    EXPECT_FALSE(response.body.empty());
    
    // Check that our headers are reflected in the response
    EXPECT_THAT(response.body, ::testing::HasSubstr("test-token"));
    EXPECT_THAT(response.body, ::testing::HasSubstr("test-value"));
}

// Test request with empty data
TEST_F(HttpClientTest, EmptyDataTest) {
    HttpClient client(5);
    
    HttpResponse response = client.make_request(
        "https://httpbin.org/get",
        "POST",
        "" // Empty data
    );
    
    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.status_code, 200);
}

// Test request with large data
TEST_F(HttpClientTest, LargeDataTest) {
    HttpClient client(10);
    
    std::string large_data(10000, 'x'); // 10KB of data
    
    HttpResponse response = client.make_request(
        "https://httpbin.org/post",
        "POST",
        large_data
    );
    
    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.status_code, 200);
}

// Test copy constructor is deleted
TEST_F(HttpClientTest, CopyConstructorDeleted) {
    HttpClient client1;
    // This should not compile, but we can test that the copy constructor is deleted
    // by checking that we can't copy the client
    EXPECT_FALSE(std::is_copy_constructible<HttpClient>::value);
}

// Test assignment operator is deleted
TEST_F(HttpClientTest, AssignmentOperatorDeleted) {
    HttpClient client1;
    HttpClient client2;
    // This should not compile, but we can test that the assignment operator is deleted
    EXPECT_FALSE(std::is_copy_assignable<HttpClient>::value);
} 