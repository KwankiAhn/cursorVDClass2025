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

# 소스 파일들
set(SOURCES
    LogFileReader.cpp
    LogParser.cpp
    LogStats.cpp
    main.cpp
)

# 헤더 파일들
set(HEADERS
    LogFileReader.hpp
    LogParser.hpp
    LogStats.hpp
)

# 실행 파일 생성
add_executable(log_analyzer ${SOURCES} ${HEADERS})

# 링크 라이브러리 (filesystem 라이브러리가 필요할 수 있음)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(log_analyzer stdc++fs)
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