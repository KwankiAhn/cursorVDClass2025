#pragma once

#include <string>
#include <string_view>
#include <chrono>
#include <optional>

/**
 * @brief 로그 엔트리 구조체
 */
struct LogEntry
{
    std::string timestamp;      // 타임스탬프
    std::string level;         // 로그 레벨 (ERROR, WARNING, INFO, DEBUG, UNKNOWN)
    std::string message;       // 로그 메시지
    std::string rawLine;       // 원본 로그 라인
    
    LogEntry() = default;
    LogEntry(std::string_view ts, std::string_view lvl, std::string_view msg, std::string_view raw)
        : timestamp(ts), level(lvl), message(msg), rawLine(raw) {}
};

/**
 * @brief 로그 파서 클래스
 * 로그 라인을 파싱하여 LogEntry 객체로 변환하는 기능을 제공
 */
class LogParser
{
public:
    /**
     * @brief 기본 생성자
     */
    LogParser() = default;
    
    /**
     * @brief 로그 라인을 파싱하여 LogEntry 객체로 변환
     * @param line 파싱할 로그 라인
     * @return LogEntry 파싱된 로그 엔트리
     */
    [[nodiscard]] LogEntry parseLine(std::string_view line) const;
    
    /**
     * @brief 로그 레벨을 감지하는 함수
     * @param line 로그 라인
     * @return std::string 감지된 로그 레벨
     */
    [[nodiscard]] std::string detectLogLevel(std::string_view line) const;
    
    /**
     * @brief 타임스탬프를 추출하는 함수
     * @param line 로그 라인
     * @return std::string 추출된 타임스탬프 (없으면 빈 문자열)
     */
    [[nodiscard]] std::string extractTimestamp(std::string_view line) const;
    
    /**
     * @brief 로그 메시지를 추출하는 함수
     * @param line 로그 라인
     * @param level 로그 레벨
     * @return std::string 추출된 메시지
     */
    [[nodiscard]] std::string extractMessage(std::string_view line, std::string_view level) const;
    
    /**
     * @brief 키워드 검색 함수
     * @param line 로그 라인
     * @param keyword 검색할 키워드
     * @return bool 키워드가 발견되면 true
     */
    [[nodiscard]] bool containsKeyword(std::string_view line, std::string_view keyword) const noexcept;
    
private:
    /**
     * @brief 문자열에서 공백을 제거하는 함수
     * @param str 입력 문자열
     * @return std::string 공백이 제거된 문자열
     */
    [[nodiscard]] std::string trim(std::string_view str) const;
}; 