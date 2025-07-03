#pragma once

#include "LogEntry.hpp"
#include <vector>
#include <map>
#include <string>
#include <chrono>

/**
 * @brief 로그 통계 데이터를 담는 구조체
 */
struct LogStatistics
{
    std::size_t totalEntries = 0;      ///< 총 엔트리 수
    std::size_t debugCount = 0;        ///< DEBUG 레벨 개수
    std::size_t infoCount = 0;         ///< INFO 레벨 개수  
    std::size_t warnCount = 0;         ///< WARN 레벨 개수
    std::size_t errorCount = 0;        ///< ERROR 레벨 개수
    std::size_t unknownCount = 0;      ///< UNKNOWN 레벨 개수
    
    double debugPercentage = 0.0;      ///< DEBUG 비율 (%)
    double infoPercentage = 0.0;       ///< INFO 비율 (%)
    double warnPercentage = 0.0;       ///< WARN 비율 (%)
    double errorPercentage = 0.0;      ///< ERROR 비율 (%)
    double unknownPercentage = 0.0;    ///< UNKNOWN 비율 (%)
    
    std::string analysisStartTime;     ///< 분석 시작 시간
    std::string analysisEndTime;       ///< 분석 완료 시간
    double analysisTimeSec = 0.0;      ///< 분석 소요 시간 (초)
};

/**
 * @brief 로그 엔트리들의 통계를 계산하고 관리하는 클래스
 */
class LogStats
{
public:
    /**
     * @brief 기본 생성자
     */
    LogStats();
    
    /**
     * @brief 소멸자
     */
    ~LogStats() = default;
    
    // 복사 및 이동 연산자 허용
    LogStats(const LogStats&) = default;
    LogStats& operator=(const LogStats&) = default;
    LogStats(LogStats&&) = default;
    LogStats& operator=(LogStats&&) = default;
    
    /**
     * @brief 로그 엔트리들의 통계 계산
     * @param entries 분석할 로그 엔트리들
     */
    void calculateStatistics(const std::vector<LogEntry>& entries);
    
    /**
     * @brief 현재 통계 정보 반환
     * @return LogStatistics 구조체
     */
    const LogStatistics& getStatistics() const noexcept;
    
    /**
     * @brief 통계 정보를 콘솔에 출력
     * @param detailed 상세 정보 출력 여부
     */
    void printStatistics(bool detailed = false) const;
    
    /**
     * @brief 통계 정보를 문자열로 반환
     * @param detailed 상세 정보 포함 여부
     * @return 통계 정보 문자열
     */
    std::string getStatisticsString(bool detailed = false) const;
    
    /**
     * @brief 특정 레벨의 개수 반환
     * @param level 조회할 로그 레벨
     * @return 해당 레벨의 개수
     */
    std::size_t getCountByLevel(LogLevel level) const noexcept;
    
    /**
     * @brief 특정 레벨의 비율 반환
     * @param level 조회할 로그 레벨
     * @return 해당 레벨의 비율 (0.0 ~ 100.0)
     */
    double getPercentageByLevel(LogLevel level) const noexcept;
    
    /**
     * @brief 총 엔트리 개수 반환
     * @return 총 엔트리 개수
     */
    std::size_t getTotalCount() const noexcept;
    
    /**
     * @brief 오류 및 경고 개수 반환
     * @return ERROR + WARN 개수
     */
    std::size_t getIssueCount() const noexcept;
    
    /**
     * @brief 심각한 문제 여부 확인 (ERROR 비율이 높은 경우)
     * @param threshold ERROR 비율 임계값 (기본 10%)
     * @return 심각한 문제가 있으면 true
     */
    bool hasCriticalIssues(double threshold = 10.0) const noexcept;
    
    /**
     * @brief 통계 초기화
     */
    void reset() noexcept;
    
    /**
     * @brief JSON 형식으로 통계 출력
     * @return JSON 형식의 통계 문자열
     */
    std::string toJson() const;

private:
    LogStatistics stats_;                           ///< 통계 데이터
    std::chrono::steady_clock::time_point startTime_;  ///< 분석 시작 시간
    std::chrono::steady_clock::time_point endTime_;    ///< 분석 완료 시간
    
    /**
     * @brief 비율 계산
     */
    void calculatePercentages();
    
    /**
     * @brief 시간 기록
     */
    void recordTiming();
    
    /**
     * @brief 시간을 문자열로 변환
     * @param timePoint 변환할 시간
     * @return 시간 문자열
     */
    std::string timeToString(const std::chrono::steady_clock::time_point& timePoint) const;
}; 