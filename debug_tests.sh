#!/bin/bash

# Debug script for C++ tests
# Usage: ./debug_tests.sh [options]

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Debug"
TEST_FILTER=""
VERBOSE=false
BREAK_ON_FAILURE=false
RUN_SINGLE_TEST=""

# Function to print usage
print_usage() {
    echo -e "${BLUE}Debug Tests Script${NC}"
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  -h, --help              Show this help message"
    echo "  -f, --filter PATTERN    Run only tests matching PATTERN"
    echo "  -v, --verbose           Enable verbose output"
    echo "  -b, --break-on-failure  Break on first test failure"
    echo "  -s, --single TEST_NAME  Run a single test (e.g., HttpUtilsTest.LogInfoTest)"
    echo "  -r, --release           Build in Release mode"
    echo ""
    echo "Examples:"
    echo "  $0                                    # Run all tests in debug mode"
    echo "  $0 -f HttpUtilsTest                   # Run all HttpUtilsTest tests"
    echo "  $0 -s HttpUtilsTest.LogInfoTest       # Run single test"
    echo "  $0 -v -b                              # Verbose with break on failure"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            print_usage
            exit 0
            ;;
        -f|--filter)
            TEST_FILTER="$2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -b|--break-on-failure)
            BREAK_ON_FAILURE=true
            shift
            ;;
        -s|--single)
            RUN_SINGLE_TEST="$2"
            shift 2
            ;;
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            print_usage
            exit 1
            ;;
    esac
done

echo -e "${BLUE}=== C++ Test Debugger ===${NC}"
echo -e "Build Type: ${YELLOW}${BUILD_TYPE}${NC}"

# Build the tests
echo -e "\n${BLUE}Building tests...${NC}"
cmake --build . --config ${BUILD_TYPE} --target api_tests

if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

echo -e "${GREEN}Build successful!${NC}"

# Prepare test arguments
TEST_ARGS=""

if [ ! -z "$TEST_FILTER" ]; then
    TEST_ARGS="$TEST_ARGS --gtest_filter=$TEST_FILTER"
    echo -e "Test Filter: ${YELLOW}$TEST_FILTER${NC}"
fi

if [ ! -z "$RUN_SINGLE_TEST" ]; then
    TEST_ARGS="$TEST_ARGS --gtest_filter=$RUN_SINGLE_TEST"
    echo -e "Single Test: ${YELLOW}$RUN_SINGLE_TEST${NC}"
fi

if [ "$VERBOSE" = true ]; then
    TEST_ARGS="$TEST_ARGS --gtest_verbose"
    echo -e "Verbose: ${YELLOW}enabled${NC}"
fi

if [ "$BREAK_ON_FAILURE" = true ]; then
    TEST_ARGS="$TEST_ARGS --gtest_break_on_failure"
    echo -e "Break on Failure: ${YELLOW}enabled${NC}"
fi

# Add useful flags for debugging
TEST_ARGS="$TEST_ARGS --gtest_color=yes"
TEST_ARGS="$TEST_ARGS --gtest_print_time=1"

echo -e "\n${BLUE}Running tests...${NC}"
echo -e "Command: ${YELLOW}./api_tests $TEST_ARGS${NC}"
echo -e "${BLUE}=====================================${NC}"

# Run the tests
./api_tests $TEST_ARGS

# Capture exit code
EXIT_CODE=$?

echo -e "${BLUE}=====================================${NC}"

if [ $EXIT_CODE -eq 0 ]; then
    echo -e "${GREEN}All tests passed! ✅${NC}"
else
    echo -e "${RED}Some tests failed! ❌${NC}"
    echo -e "\n${YELLOW}Debugging Tips:${NC}"
    echo "1. Use VS Code debugger with 'Debug Tests with Break on Failure'"
    echo "2. Add breakpoints in your test files"
    echo "3. Use --gtest_filter to run specific tests"
    echo "4. Check the test output above for failure details"
fi

exit $EXIT_CODE 