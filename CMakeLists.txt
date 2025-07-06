###
### CMake settings
###
# see http://www.cmake.org/Wiki/CMake_Policies

cmake_minimum_required(VERSION 3.5)

project(cpp-interview-prep)

# Generate compile_commands.json for better IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(boosttest src/boosttest.cpp)
endif()

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

# Find nlohmann/json (header-only library)
find_package(nlohmann_json 3.2.0 QUIET)
if (NOT nlohmann_json_FOUND)
    # If not found, try to find it in common locations
    find_path(JSON_INCLUDE_DIR nlohmann/json.hpp
        PATHS
        /usr/include
        /usr/local/include
        /opt/homebrew/include
        /opt/local/include
    )
    if(JSON_INCLUDE_DIR)
        set(nlohmann_json_FOUND TRUE)
        set(nlohmann_json_INCLUDE_DIRS ${JSON_INCLUDE_DIR})
    endif()
endif()

# Find Google Test
find_package(GTest QUIET)
if (NOT GTest_FOUND)
    # Try to find GTest in common locations
    find_path(GTEST_INCLUDE_DIR gtest/gtest.h
        PATHS
        /usr/include
        /usr/local/include
        /opt/homebrew/include
        /opt/local/include
    )
    find_library(GTEST_LIBRARY gtest
        PATHS
        /usr/lib
        /usr/local/lib
        /opt/homebrew/lib
        /opt/local/lib
    )
    find_library(GTEST_MAIN_LIBRARY gtest_main
        PATHS
        /usr/lib
        /usr/local/lib
        /opt/homebrew/lib
        /opt/local/lib
    )
    if(GTEST_INCLUDE_DIR AND GTEST_LIBRARY AND GTEST_MAIN_LIBRARY)
        set(GTest_FOUND TRUE)
        set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR})
        set(GTEST_LIBRARIES ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})
    endif()
endif()

add_executable(hello src/helloworld.cpp)
target_link_libraries(hello PRIVATE ${CURL_LIBRARIES})

add_executable(sampleapi 
    src/sampleapi.cpp 
    src/HttpClient.cpp 
    src/HttpUtils.cpp
)

if(nlohmann_json_FOUND)
    target_include_directories(sampleapi PRIVATE ${nlohmann_json_INCLUDE_DIRS})
endif()

target_include_directories(sampleapi PRIVATE include)
target_link_libraries(sampleapi PRIVATE ${CURL_LIBRARIES})

# Add test executable if GTest is found
if(GTest_FOUND)
    add_executable(api_tests
        tests/TestMain.cpp
        tests/HttpUtilsTest.cpp
        tests/HttpClientTest.cpp
        tests/ApiExceptionTest.cpp
        tests/SampleApiTest.cpp
        src/HttpClient.cpp
        src/HttpUtils.cpp
    )
    
    target_include_directories(api_tests PRIVATE include)
    target_include_directories(api_tests PRIVATE ${GTEST_INCLUDE_DIRS})
    target_include_directories(api_tests PRIVATE ${nlohmann_json_INCLUDE_DIRS})
    target_link_libraries(api_tests PRIVATE ${CURL_LIBRARIES} ${GTEST_LIBRARIES})
    
    # Enable CTest integration
    enable_testing()
    add_test(NAME API_Tests COMMAND api_tests)
    
    message(STATUS "Google Test found - test target 'api_tests' created")
else()
    message(WARNING "Google Test not found - skipping test target creation")
endif()
