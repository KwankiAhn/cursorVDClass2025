cmake_minimum_required(VERSION 3.15)
project(LogAnalyzer CXX)

# C++17 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 컴파일러 플래그 설정
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()

# Catch2 다운로드 및 설정
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

# 소스 파일들
set(SOURCES
    LogFileReader.cpp
    LogParser.cpp
    LogStats.cpp
)

# 헤더 파일들
set(HEADERS
    LogFileReader.hpp
    LogParser.hpp
    LogStats.hpp
)

# 라이브러리 생성 (테스트에서 재사용하기 위해)
add_library(log_analyzer_lib ${SOURCES} ${HEADERS})

# 링크 라이브러리 (filesystem 라이브러리가 필요할 수 있음)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(log_analyzer_lib stdc++fs)
endif()

# 메인 실행 파일
add_executable(log_analyzer main.cpp)
target_link_libraries(log_analyzer log_analyzer_lib)

# 테스트 실행 파일
add_executable(log_analyzer_tests 
    tests/test_main.cpp
    tests/test_log_file_reader.cpp
    tests/test_log_parser.cpp
    tests/test_log_stats.cpp
)

target_link_libraries(log_analyzer_tests 
    log_analyzer_lib 
    Catch2::Catch2WithMain
)

# 테스트 활성화
enable_testing()
add_test(NAME log_analyzer_unit_tests COMMAND log_analyzer_tests)

# CTest 설정
include(CTest)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
if(EXISTS ${catch2_SOURCE_DIR}/extras/Catch.cmake)
    include(Catch)
    catch_discover_tests(log_analyzer_tests)
endif()

# 설치 설정
install(TARGETS log_analyzer DESTINATION bin)

# 빌드 타입별 기본 설정
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Testing enabled: ${BUILD_TESTING}") 