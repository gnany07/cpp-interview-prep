#include <gtest/gtest.h>
#include "ApiException.h"
#include <stdexcept>

class ApiExceptionTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Test ApiException constructor with message only
TEST_F(ApiExceptionTest, ConstructorWithMessageOnly) {
    std::string message = "Test API error";
    
    EXPECT_NO_THROW({
        ApiException exception(message);
    });
    
    ApiException exception(message);
    EXPECT_EQ(exception.what(), message);
    EXPECT_EQ(exception.get_status_code(), 0); // Default status code
}

// Test ApiException constructor with message and status code
TEST_F(ApiExceptionTest, ConstructorWithMessageAndStatusCode) {
    std::string message = "Test API error";
    int status_code = 404;
    
    EXPECT_NO_THROW({
        ApiException exception(message, status_code);
    });
    
    ApiException exception(message, status_code);
    EXPECT_EQ(exception.what(), message);
    EXPECT_EQ(exception.get_status_code(), status_code);
}

// Test ApiException with different status codes
TEST_F(ApiExceptionTest, DifferentStatusCodes) {
    std::string message = "Test error";
    
    ApiException exception400(message, 400);
    EXPECT_EQ(exception400.get_status_code(), 400);
    
    ApiException exception401(message, 401);
    EXPECT_EQ(exception401.get_status_code(), 401);
    
    ApiException exception404(message, 404);
    EXPECT_EQ(exception404.get_status_code(), 404);
    
    ApiException exception500(message, 500);
    EXPECT_EQ(exception500.get_status_code(), 500);
    
    ApiException exception503(message, 503);
    EXPECT_EQ(exception503.get_status_code(), 503);
}

// Test ApiException inheritance from std::runtime_error
TEST_F(ApiExceptionTest, InheritanceTest) {
    std::string message = "Test inheritance";
    ApiException exception(message, 500);
    
    // Should be able to catch as std::runtime_error
    try {
        throw exception;
    } catch (const std::runtime_error& e) {
        EXPECT_EQ(e.what(), message);
    }
    
    // Should be able to catch as ApiException
    try {
        throw exception;
    } catch (const ApiException& e) {
        EXPECT_EQ(e.what(), message);
        EXPECT_EQ(e.get_status_code(), 500);
    }
}

// Test ApiException with empty message
TEST_F(ApiExceptionTest, EmptyMessage) {
    std::string empty_message = "";
    
    ApiException exception(empty_message, 400);
    EXPECT_EQ(exception.what(), empty_message);
    EXPECT_EQ(exception.get_status_code(), 400);
}

// Test ApiException with special characters in message
TEST_F(ApiExceptionTest, SpecialCharactersInMessage) {
    std::string special_message = "Error with special chars: !@#$%^&*()_+-=[]{}|;':\",./<>?";
    
    ApiException exception(special_message, 500);
    EXPECT_EQ(exception.what(), special_message);
    EXPECT_EQ(exception.get_status_code(), 500);
}

// Test ApiException with very long message
TEST_F(ApiExceptionTest, LongMessage) {
    std::string long_message(1000, 'x'); // 1000 character message
    
    ApiException exception(long_message, 400);
    EXPECT_EQ(exception.what(), long_message);
    EXPECT_EQ(exception.get_status_code(), 400);
}

// Test ApiException with negative status code
TEST_F(ApiExceptionTest, NegativeStatusCode) {
    std::string message = "Test with negative status";
    int negative_status = -1;
    
    ApiException exception(message, negative_status);
    EXPECT_EQ(exception.what(), message);
    EXPECT_EQ(exception.get_status_code(), negative_status);
}

// Test ApiException with zero status code
TEST_F(ApiExceptionTest, ZeroStatusCode) {
    std::string message = "Test with zero status";
    int zero_status = 0;
    
    ApiException exception(message, zero_status);
    EXPECT_EQ(exception.what(), message);
    EXPECT_EQ(exception.get_status_code(), zero_status);
}

// Test ApiException with very large status code
TEST_F(ApiExceptionTest, LargeStatusCode) {
    std::string message = "Test with large status";
    int large_status = 999999;
    
    ApiException exception(message, large_status);
    EXPECT_EQ(exception.what(), message);
    EXPECT_EQ(exception.get_status_code(), large_status);
}

// Test ApiException const correctness
TEST_F(ApiExceptionTest, ConstCorrectness) {
    std::string message = "Test const correctness";
    const ApiException exception(message, 500);
    
    // Should be able to call get_status_code() on const object
    EXPECT_EQ(exception.get_status_code(), 500);
    EXPECT_EQ(exception.what(), message);
}

// Test ApiException copy constructor
TEST_F(ApiExceptionTest, CopyConstructor) {
    std::string message = "Test copy constructor";
    ApiException original(message, 404);
    
    ApiException copy(original);
    
    EXPECT_EQ(copy.what(), original.what());
    EXPECT_EQ(copy.get_status_code(), original.get_status_code());
    EXPECT_EQ(copy.what(), message);
    EXPECT_EQ(copy.get_status_code(), 404);
}

// Test ApiException assignment operator
TEST_F(ApiExceptionTest, AssignmentOperator) {
    std::string message1 = "Original message";
    std::string message2 = "Assigned message";
    
    ApiException original(message1, 400);
    ApiException assigned(message2, 500);
    
    assigned = original;
    
    EXPECT_EQ(assigned.what(), original.what());
    EXPECT_EQ(assigned.get_status_code(), original.get_status_code());
    EXPECT_EQ(assigned.what(), message1);
    EXPECT_EQ(assigned.get_status_code(), 400);
}

// Test ApiException move semantics (if supported)
TEST_F(ApiExceptionTest, MoveSemantics) {
    std::string message = "Test move semantics";
    ApiException original(message, 503);
    
    // Test move constructor
    ApiException moved(std::move(original));
    
    EXPECT_EQ(moved.what(), message);
    EXPECT_EQ(moved.get_status_code(), 503);
}

// Test ApiException in exception handling scenarios
TEST_F(ApiExceptionTest, ExceptionHandling) {
    std::string message = "Test exception handling";
    int status_code = 429;
    
    try {
        throw ApiException(message, status_code);
    } catch (const ApiException& e) {
        EXPECT_EQ(e.what(), message);
        EXPECT_EQ(e.get_status_code(), status_code);
    } catch (...) {
        FAIL() << "Expected ApiException but got different exception type";
    }
}

// Test ApiException with multiple exceptions
TEST_F(ApiExceptionTest, MultipleExceptions) {
    std::vector<ApiException> exceptions;
    
    exceptions.emplace_back("Error 1", 400);
    exceptions.emplace_back("Error 2", 401);
    exceptions.emplace_back("Error 3", 404);
    exceptions.emplace_back("Error 4", 500);
    
    EXPECT_EQ(exceptions.size(), 4);
    EXPECT_EQ(exceptions[0].get_status_code(), 400);
    EXPECT_EQ(exceptions[1].get_status_code(), 401);
    EXPECT_EQ(exceptions[2].get_status_code(), 404);
    EXPECT_EQ(exceptions[3].get_status_code(), 500);
} 