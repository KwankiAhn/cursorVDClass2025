#pragma once

#include <string>
#include <chrono>
#include <iostream>

/**
 * @brief 로그 레벨을 나타내는 열거형
 */
enum class LogLevel
{
    UNKNOWN = 0,    ///< 알 수 없는 레벨
    DEBUG = 1,      ///< 디버그 정보
    INFO = 2,       ///< 일반 정보
    WARN = 3,       ///< 경고
    ERROR = 4       ///< 오류
};

/**
 * @brief 파싱된 로그 엔트리를 표현하는 구조체
 */
struct LogEntry
{
    std::string timestamp;      ///< 타임스탬프 문자열
    LogLevel level;            ///< 로그 레벨
    std::string message;       ///< 로그 메시지
    std::size_t lineNumber;    ///< 원본 파일의 라인 번호
    
    /**
     * @brief 기본 생성자
     */
    LogEntry() 
        : level(LogLevel::UNKNOWN)
        , lineNumber(0)
    {
    }
    
    /**
     * @brief 매개변수를 받는 생성자
     */
    LogEntry(const std::string& ts, LogLevel lvl, const std::string& msg, std::size_t line = 0)
        : timestamp(ts)
        , level(lvl) 
        , message(msg)
        , lineNumber(line)
    {
    }
    
    /**
     * @brief 로그 엔트리가 유효한지 확인
     * @return 유효하면 true
     */
    bool isValid() const noexcept
    {
        return level != LogLevel::UNKNOWN && !message.empty();
    }
    
    /**
     * @brief 지정된 키워드를 포함하는지 확인
     * @param keyword 검색할 키워드
     * @return 키워드를 포함하면 true
     */
    bool containsKeyword(const std::string& keyword) const noexcept
    {
        if (keyword.empty())
        {
            return false;
        }
        
        // 대소문자 구분 없이 검색
        return message.find(keyword) != std::string::npos ||
               timestamp.find(keyword) != std::string::npos;
    }
};

/**
 * @brief 로그 레벨을 문자열로 변환
 * @param level 로그 레벨
 * @return 레벨 문자열
 */
inline std::string logLevelToString(LogLevel level)
{
    switch (level)
    {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARN:    return "WARN";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::UNKNOWN: 
        default:                return "UNKNOWN";
    }
}

/**
 * @brief 문자열을 로그 레벨로 변환
 * @param levelStr 레벨 문자열
 * @return 로그 레벨
 */
inline LogLevel stringToLogLevel(const std::string& levelStr)
{
    if (levelStr == "DEBUG") return LogLevel::DEBUG;
    if (levelStr == "INFO")  return LogLevel::INFO;
    if (levelStr == "WARN" || levelStr == "WARNING") return LogLevel::WARN;
    if (levelStr == "ERROR") return LogLevel::ERROR;
    
    return LogLevel::UNKNOWN;
}

/**
 * @brief LogEntry 출력 연산자
 */
inline std::ostream& operator<<(std::ostream& os, const LogEntry& entry)
{
    os << "[" << logLevelToString(entry.level) << "] " 
       << entry.timestamp << " " << entry.message;
    return os;
} 