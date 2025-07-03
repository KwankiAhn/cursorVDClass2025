cmake_minimum_required(VERSION 3.20)
project(LogAnalyzer VERSION 1.0.0 LANGUAGES CXX)

# C++17 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 컴파일러 경고 옵션 설정
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 릴리즈 모드에서 최적화 설정
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# 라이브러리 생성 (LogAnalyzer)
add_library(LogAnalyzer
    LogFileReader.cpp
    LogFileReader.hpp
    LogParser.cpp
    LogParser.hpp
    LogStats.cpp
    LogStats.hpp
    LogFileWatcher.cpp
    LogFileWatcher.hpp
    HTMLReporter.cpp
    HTMLReporter.hpp
)

# 라이브러리 속성 설정
target_compile_features(LogAnalyzer PUBLIC cxx_std_17)

# nlohmann/json 헤더 파일 경로 추가
target_include_directories(LogAnalyzer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 헤더 파일 포함 디렉토리 설정
target_include_directories(LogAnalyzer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# 멀티스레드 지원
find_package(Threads REQUIRED)
target_link_libraries(LogAnalyzer PUBLIC Threads::Threads)

# 메인 실행 파일 생성 (배치 분석용)
add_executable(log_analyzer main.cpp)

# 실시간 감시 실행 파일 생성
add_executable(log_analyzer_live main_live.cpp)

# 간단한 실시간 감시 프로그램 
add_executable(simple_log_monitor simple_log_monitor.cpp)

# LogAnalyzer 라이브러리 링크
target_link_libraries(log_analyzer PRIVATE LogAnalyzer)
target_link_libraries(log_analyzer_live PRIVATE LogAnalyzer)

# 간단한 모니터 링크 (독립 실행)
target_link_libraries(simple_log_monitor PRIVATE Threads::Threads)
target_include_directories(simple_log_monitor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# std::filesystem 링크 (일부 컴파일러에서 필요)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(LogAnalyzer PUBLIC stdc++fs)
    target_link_libraries(log_analyzer PRIVATE stdc++fs)
    target_link_libraries(log_analyzer_live PRIVATE stdc++fs)
    target_link_libraries(simple_log_monitor PRIVATE stdc++fs)
endif()

# 빌드 타겟 설정
set_target_properties(log_analyzer log_analyzer_live simple_log_monitor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# 외부 라이브러리 연동
include(FetchContent)

# Catch2 테스트 프레임워크
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)

# nlohmann/json 라이브러리 (헤더 파일 다운로드 방식)
set(NLOHMANN_JSON_HEADER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/nlohmann/json.hpp")
set(NLOHMANN_JSON_URL "https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp")

if(NOT EXISTS ${NLOHMANN_JSON_HEADER_FILE})
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/nlohmann")
    message(STATUS "Downloading nlohmann/json header...")
    file(DOWNLOAD ${NLOHMANN_JSON_URL} ${NLOHMANN_JSON_HEADER_FILE})
endif()

FetchContent_MakeAvailable(Catch2)

# 테스트 실행 파일 생성
add_executable(log_analyzer_tests
    tests/test_main.cpp
    tests/test_log_parser.cpp
    tests/test_log_file_reader.cpp
    tests/test_log_stats.cpp
    tests/test_integration.cpp
    tests/test_main_functionality.cpp
)

# 테스트 실행 파일에 라이브러리 링크
target_link_libraries(log_analyzer_tests PRIVATE 
    LogAnalyzer
    Catch2::Catch2WithMain
)

# 테스트 활성화
enable_testing()
add_test(NAME UnitTests COMMAND log_analyzer_tests)
add_test(NAME BasicTest COMMAND log_analyzer ../../sample-logs/sample.log) 