cmake_minimum_required(VERSION 3.17)

# 프로젝트 정의
project(LogAnalyzer 
    VERSION 1.0.0 
    DESCRIPTION "C++17 기반 로그 분석 도구"
    LANGUAGES CXX)

# FetchContent 모듈 포함 (Catch2 다운로드용)
include(FetchContent)

# C++17 표준 요구
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 컴파일러별 설정
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # GCC 또는 Clang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Microsoft Visual C++
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
endif()

# 소스 파일들
set(SOURCES
    main.cpp
    LogFileReader.cpp
    LogParser.cpp
    LogStats.cpp
)

# 헤더 파일들
set(HEADERS
    LogFileReader.hpp
    LogEntry.hpp
    LogParser.hpp
    LogStats.hpp
)

# 실행 파일 생성
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 빌드 타입이 지정되지 않은 경우 Release로 설정
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "빌드 타입 선택" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# 출력 디렉토리 설정
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 설치 규칙 (선택적)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Catch2 다운로드 및 설정
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0  # 안정적인 버전 사용
)

FetchContent_MakeAvailable(Catch2)

# CTest 활성화
enable_testing()

# 테스트 소스 파일들
set(TEST_SOURCES
    tests/test_main.cpp
    tests/test_log_file_reader.cpp
    tests/test_log_parser.cpp
    tests/test_log_stats.cpp
)

# 라이브러리 생성 (테스트에서 공유하기 위해)
add_library(LogAnalyzerLib 
    LogFileReader.cpp
    LogParser.cpp
    LogStats.cpp
)

# 라이브러리 헤더 디렉토리 설정
target_include_directories(LogAnalyzerLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 라이브러리에 플랫폼별 링크 라이브러리 설정
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(LogAnalyzerLib stdc++fs)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(LogAnalyzerLib c++fs)
endif()

# 메인 실행 파일을 라이브러리에 링크
target_link_libraries(${PROJECT_NAME} LogAnalyzerLib)

# 테스트 실행 파일 생성
add_executable(log_analyzer_tests ${TEST_SOURCES})

# 테스트 실행 파일을 라이브러리와 Catch2에 링크
target_link_libraries(log_analyzer_tests LogAnalyzerLib Catch2::Catch2WithMain)

# 테스트 출력 디렉토리 설정
set_target_properties(log_analyzer_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# CTest에 테스트 등록
add_test(NAME LogAnalyzerTests COMMAND log_analyzer_tests)

# 빌드 정보 출력
message(STATUS "=== LogAnalyzer 빌드 구성 ===")
message(STATUS "프로젝트 이름: ${PROJECT_NAME}")
message(STATUS "버전: ${PROJECT_VERSION}")
message(STATUS "C++ 표준: C++${CMAKE_CXX_STANDARD}")
message(STATUS "컴파일러: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "빌드 타입: ${CMAKE_BUILD_TYPE}")
message(STATUS "출력 디렉토리: ${CMAKE_BINARY_DIR}/bin")
message(STATUS "테스트 프레임워크: Catch2 v3.4.0")
message(STATUS "===============================") 