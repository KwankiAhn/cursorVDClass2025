#pragma once

#include "LogEntry.hpp"
#include <string>
#include <vector>
#include <optional>
#include <regex>

/**
 * @brief 로그 라인을 파싱하여 LogEntry 객체로 변환하는 클래스
 * 
 * 다양한 로그 형식을 지원하며, 키워드 검색 기능도 제공합니다.
 */
class LogParser
{
public:
    /**
     * @brief 기본 생성자
     */
    LogParser();
    
    /**
     * @brief 소멸자
     */
    ~LogParser() = default;
    
    // 복사 및 이동 연산자 허용
    LogParser(const LogParser&) = default;
    LogParser& operator=(const LogParser&) = default;
    LogParser(LogParser&&) = default;
    LogParser& operator=(LogParser&&) = default;
    
    /**
     * @brief 로그 라인을 파싱하여 LogEntry로 변환
     * @param line 파싱할 로그 라인
     * @param lineNumber 라인 번호 (선택적)
     * @return 파싱된 LogEntry, 실패 시 std::nullopt
     */
    std::optional<LogEntry> parseLine(const std::string& line, std::size_t lineNumber = 0) const;
    
    /**
     * @brief 여러 로그 라인을 한 번에 파싱
     * @param lines 파싱할 로그 라인들
     * @return 파싱된 LogEntry 벡터
     */
    std::vector<LogEntry> parseLines(const std::vector<std::string>& lines) const;
    
    /**
     * @brief 키워드를 포함하는 로그 엔트리들만 필터링
     * @param entries 대상 로그 엔트리들
     * @param keyword 검색할 키워드
     * @return 키워드를 포함하는 엔트리들
     */
    std::vector<LogEntry> filterByKeyword(const std::vector<LogEntry>& entries, 
                                         const std::string& keyword) const;
    
    /**
     * @brief 특정 로그 레벨의 엔트리들만 필터링
     * @param entries 대상 로그 엔트리들
     * @param level 필터링할 로그 레벨
     * @return 해당 레벨의 엔트리들
     */
    std::vector<LogEntry> filterByLevel(const std::vector<LogEntry>& entries, 
                                       LogLevel level) const;
    
    /**
     * @brief 최소 로그 레벨 이상의 엔트리들만 필터링
     * @param entries 대상 로그 엔트리들
     * @param minLevel 최소 로그 레벨
     * @return 최소 레벨 이상의 엔트리들
     */
    std::vector<LogEntry> filterByMinLevel(const std::vector<LogEntry>& entries, 
                                          LogLevel minLevel) const;
    
    /**
     * @brief 파싱 성공률 반환
     * @return 파싱 성공률 (0.0 ~ 1.0)
     */
    double getParseSuccessRate() const noexcept;
    
    /**
     * @brief 파싱 통계 초기화
     */
    void resetStatistics() noexcept;

private:
    mutable std::size_t totalParsed_;        ///< 총 파싱 시도 횟수
    mutable std::size_t successfulParsed_;   ///< 성공한 파싱 횟수
    
    std::regex timestampPattern_;            ///< 타임스탬프 패턴
    std::regex logLevelPattern_;            ///< 로그 레벨 패턴
    
    /**
     * @brief 타임스탬프 추출
     * @param line 로그 라인
     * @return 추출된 타임스탬프, 실패 시 빈 문자열
     */
    std::string extractTimestamp(const std::string& line) const;
    
    /**
     * @brief 로그 레벨 추출
     * @param line 로그 라인
     * @return 추출된 로그 레벨
     */
    LogLevel extractLogLevel(const std::string& line) const;
    
    /**
     * @brief 로그 메시지 추출
     * @param line 로그 라인
     * @param timestampEnd 타임스탬프 끝 위치
     * @param levelEnd 레벨 끝 위치
     * @return 추출된 메시지
     */
    std::string extractMessage(const std::string& line, 
                              std::size_t timestampEnd, 
                              std::size_t levelEnd) const;
    
    /**
     * @brief 정규식 패턴 초기화
     */
    void initializePatterns();
}; 