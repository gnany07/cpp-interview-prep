#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char **argv) {
    std::cout << "Running API Integration Unit Tests..." << std::endl;
    std::cout << "=====================================" << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    
    int result = RUN_ALL_TESTS();
    
    std::cout << "=====================================" << std::endl;
    if (result == 0) {
        std::cout << "All tests passed! ✅" << std::endl;
    } else {
        std::cout << "Some tests failed! ❌" << std::endl;
    }
    
    return result;
} 