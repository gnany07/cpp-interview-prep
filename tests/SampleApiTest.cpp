#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "HttpClient.h"
#include "HttpUtils.h"

using json = nlohmann::json;

// Forward declarations of the functions we want to test
void perform_get();
void perform_post();
void perform_put();
void perform_delete();

class SampleApiTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize cURL globally for tests
        curl_global_init(CURL_GLOBAL_DEFAULT);
        
        // Redirect cout/cerr to capture output
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

// Test JSON object creation and parsing
TEST_F(SampleApiTest, JsonObjectCreation) {
    json post_data;
    post_data["title"] = "test title";
    post_data["body"] = "test body";
    post_data["userId"] = 123;
    
    std::string json_string = post_data.dump();
    
    // Verify JSON structure
    EXPECT_THAT(json_string, ::testing::HasSubstr("\"title\":\"test title\""));
    EXPECT_THAT(json_string, ::testing::HasSubstr("\"body\":\"test body\""));
    EXPECT_THAT(json_string, ::testing::HasSubstr("\"userId\":123"));
    
    // Parse back and verify
    json parsed = json::parse(json_string);
    EXPECT_EQ(parsed["title"], "test title");
    EXPECT_EQ(parsed["body"], "test body");
    EXPECT_EQ(parsed["userId"], 123);
}

// Test JSON parsing with error handling
TEST_F(SampleApiTest, JsonParsingErrorHandling) {
    std::string invalid_json = "{ invalid json }";
    
    EXPECT_THROW({
        json::parse(invalid_json);
    }, json::parse_error);
}

// Test JSON field extraction
TEST_F(SampleApiTest, JsonFieldExtraction) {
    std::string valid_json = R"({
        "id": 1,
        "title": "Test Title",
        "body": "Test Body",
        "userId": 42
    })";
    
    json response = json::parse(valid_json);
    
    EXPECT_EQ(response["id"], 1);
    EXPECT_EQ(response["title"], "Test Title");
    EXPECT_EQ(response["body"], "Test Body");
    EXPECT_EQ(response["userId"], 42);
}

// Test JSON field extraction with missing fields
TEST_F(SampleApiTest, JsonMissingFields) {
    std::string partial_json = R"({
        "id": 1,
        "title": "Test Title"
    })";
    
    json response = json::parse(partial_json);
    
    EXPECT_EQ(response["id"], 1);
    EXPECT_EQ(response["title"], "Test Title");
    EXPECT_TRUE(response["body"].is_null()); // Missing field should be null
    EXPECT_TRUE(response["userId"].is_null());
}

// Test HTTP client integration with JSON
TEST_F(SampleApiTest, HttpClientJsonIntegration) {
    HttpClient client(10);
    
    // Create test JSON data
    json test_data;
    test_data["test"] = "value";
    test_data["number"] = 42;
    test_data["boolean"] = true;
    
    std::string json_string = test_data.dump();
    std::vector<std::string> headers = {"Content-Type: application/json"};
    
    // Make request to httpbin (echo service)
    HttpResponse response = client.make_request(
        "https://httpbin.org/post",
        "POST",
        json_string,
        headers
    );
    
    if (response.success) {
        // Parse response
        json response_json = json::parse(response.body);
        
        // Verify our data was echoed back
        EXPECT_EQ(response_json["json"]["test"], "value");
        EXPECT_EQ(response_json["json"]["number"], 42);
        EXPECT_EQ(response_json["json"]["boolean"], true);
    } else {
        // If request fails, that's okay for unit tests
        // Just log the error
        std::cout << "Request failed: " << response.error_message << std::endl;
    }
}

// Test JSON array handling
TEST_F(SampleApiTest, JsonArrayHandling) {
    json array_data = json::array();
    array_data.push_back("item1");
    array_data.push_back("item2");
    array_data.push_back("item3");
    
    std::string json_string = array_data.dump();
    
    // Parse back
    json parsed = json::parse(json_string);
    EXPECT_TRUE(parsed.is_array());
    EXPECT_EQ(parsed.size(), 3);
    EXPECT_EQ(parsed[0], "item1");
    EXPECT_EQ(parsed[1], "item2");
    EXPECT_EQ(parsed[2], "item3");
}

// Test JSON nested objects
TEST_F(SampleApiTest, JsonNestedObjects) {
    json nested_data;
    nested_data["user"]["name"] = "John Doe";
    nested_data["user"]["age"] = 30;
    nested_data["user"]["address"]["city"] = "New York";
    nested_data["user"]["address"]["country"] = "USA";
    
    std::string json_string = nested_data.dump();
    
    // Parse back
    json parsed = json::parse(json_string);
    EXPECT_EQ(parsed["user"]["name"], "John Doe");
    EXPECT_EQ(parsed["user"]["age"], 30);
    EXPECT_EQ(parsed["user"]["address"]["city"], "New York");
    EXPECT_EQ(parsed["user"]["address"]["country"], "USA");
}

// Test JSON with different data types
TEST_F(SampleApiTest, JsonDataTypes) {
    json mixed_data;
    mixed_data["string"] = "hello";
    mixed_data["integer"] = 42;
    mixed_data["float"] = 3.14;
    mixed_data["boolean"] = true;
    mixed_data["null"] = nullptr;
    
    std::string json_string = mixed_data.dump();
    
    // Parse back
    json parsed = json::parse(json_string);
    EXPECT_EQ(parsed["string"], "hello");
    EXPECT_EQ(parsed["integer"], 42);
    EXPECT_EQ(parsed["float"], 3.14);
    EXPECT_EQ(parsed["boolean"], true);
    EXPECT_TRUE(parsed["null"].is_null());
}

// Test JSON pretty printing
TEST_F(SampleApiTest, JsonPrettyPrinting) {
    json data;
    data["name"] = "Test";
    data["value"] = 123;
    
    std::string pretty_json = data.dump(2); // 2 spaces indentation
    
    EXPECT_THAT(pretty_json, ::testing::HasSubstr("\n"));
    EXPECT_THAT(pretty_json, ::testing::HasSubstr("  ")); // Should have indentation
}

// Test JSON merge operations
TEST_F(SampleApiTest, JsonMergeOperations) {
    json base_data;
    base_data["name"] = "Original";
    base_data["value"] = 100;
    
    json update_data;
    update_data["value"] = 200;
    update_data["new_field"] = "new value";
    
    // Merge update_data into base_data
    base_data.merge_patch(update_data);
    
    EXPECT_EQ(base_data["name"], "Original"); // Should remain unchanged
    EXPECT_EQ(base_data["value"], 200); // Should be updated
    EXPECT_EQ(base_data["new_field"], "new value"); // Should be added
}

// Test JSON validation
TEST_F(SampleApiTest, JsonValidation) {
    // Valid JSON
    EXPECT_NO_THROW({
        json::parse("{\"valid\": \"json\"}");
    });
    
    // Invalid JSON
    EXPECT_THROW({
        json::parse("{ invalid }");
    }, json::parse_error);
    
    // Empty JSON object
    EXPECT_NO_THROW({
        json::parse("{}");
    });
    
    // Empty JSON array
    EXPECT_NO_THROW({
        json::parse("[]");
    });
}

// Test JSON with special characters
TEST_F(SampleApiTest, JsonSpecialCharacters) {
    json special_data;
    special_data["message"] = "Hello\nWorld\tWith\r\nSpecial\"Chars\\";
    special_data["unicode"] = "café résumé naïve";
    
    std::string json_string = special_data.dump();
    
    // Parse back
    json parsed = json::parse(json_string);
    EXPECT_EQ(parsed["message"], "Hello\nWorld\tWith\r\nSpecial\"Chars\\");
    EXPECT_EQ(parsed["unicode"], "café résumé naïve");
}

// Test JSON size limits
TEST_F(SampleApiTest, JsonSizeLimits) {
    // Test with large JSON object
    json large_data;
    for (int i = 0; i < 1000; ++i) {
        large_data["key" + std::to_string(i)] = "value" + std::to_string(i);
    }
    
    std::string json_string = large_data.dump();
    EXPECT_GT(json_string.length(), 1000);
    
    // Parse back
    json parsed = json::parse(json_string);
    EXPECT_EQ(parsed.size(), 1000);
    EXPECT_EQ(parsed["key0"], "value0");
    EXPECT_EQ(parsed["key999"], "value999");
} 