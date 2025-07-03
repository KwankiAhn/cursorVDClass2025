#pragma once

#include "LogParser.hpp"
#include <map>
#include <string>
#include <vector>
#include <chrono>

/**
 * @brief 로그 통계 정보를 저장하는 구조체
 */
struct LogStatistics
{
    size_t totalLines = 0;                             // 전체 라인 수
    std::map<std::string, size_t> levelCounts;         // 로그 레벨별 개수
    std::map<std::string, size_t> keywordCounts;       // 키워드별 개수
    std::vector<LogEntry> errorEntries;                // 에러 로그 엔트리들
    std::vector<LogEntry> keywordMatches;              // 키워드 매칭 엔트리들
    std::string mostCommonLevel;                       // 가장 많은 로그 레벨
    double processingTimeMs = 0.0;                     // 처리 시간 (밀리초)
    
    LogStatistics()
    {
        // 로그 레벨 초기화
        levelCounts["ERROR"] = 0;
        levelCounts["WARNING"] = 0;
        levelCounts["INFO"] = 0;
        levelCounts["DEBUG"] = 0;
        levelCounts["UNKNOWN"] = 0;
    }
};

/**
 * @brief 로그 통계 계산 클래스
 * LogEntry를 받아서 다양한 통계를 계산하는 기능을 제공
 */
class LogStats
{
public:
    /**
     * @brief 기본 생성자
     */
    LogStats() = default;
    
    /**
     * @brief 로그 엔트리를 추가하여 통계 업데이트
     * @param entry 추가할 로그 엔트리
     */
    void addEntry(const LogEntry& entry);
    
    /**
     * @brief 키워드 매칭을 위한 로그 엔트리 추가
     * @param entry 추가할 로그 엔트리
     * @param keyword 검색할 키워드
     */
    void addEntryWithKeyword(const LogEntry& entry, const std::string& keyword);
    
    /**
     * @brief 통계 계산 완료 및 최종 처리
     */
    void finalize();
    
    /**
     * @brief 현재 통계 정보 반환
     * @return const LogStatistics& 통계 정보 참조
     */
    [[nodiscard]] const LogStatistics& getStatistics() const noexcept;
    
    /**
     * @brief 특정 로그 레벨의 개수 반환
     * @param level 로그 레벨
     * @return size_t 개수
     */
    [[nodiscard]] size_t getCountForLevel(const std::string& level) const;
    
    /**
     * @brief 특정 로그 레벨의 비율 반환
     * @param level 로그 레벨
     * @return double 비율 (0.0 ~ 100.0)
     */
    [[nodiscard]] double getPercentageForLevel(const std::string& level) const;
    
    /**
     * @brief 가장 많은 로그 레벨 반환
     * @return std::string 가장 많은 로그 레벨
     */
    [[nodiscard]] std::string getMostCommonLevel() const;
    
    /**
     * @brief 에러 로그 엔트리들 반환
     * @return const std::vector<LogEntry>& 에러 로그 엔트리들
     */
    [[nodiscard]] const std::vector<LogEntry>& getErrorEntries() const noexcept;
    
    /**
     * @brief 키워드 매칭 엔트리들 반환
     * @return const std::vector<LogEntry>& 키워드 매칭 엔트리들
     */
    [[nodiscard]] const std::vector<LogEntry>& getKeywordMatches() const noexcept;
    
    /**
     * @brief 통계 정보 초기화
     */
    void reset();
    
    /**
     * @brief 처리 시간 설정
     * @param timeMs 처리 시간 (밀리초)
     */
    void setProcessingTime(double timeMs);
    
         /**
      * @brief 통계 정보를 콘솔에 출력
      */
     void printStatistics() const;
     
     /**
      * @brief 통계 정보를 JSON 형태로 변환
      * @return std::string JSON 문자열
      */
     [[nodiscard]] std::string toJson() const;
     
     /**
      * @brief 통계 정보를 JSON 파일로 저장
      * @param filename 저장할 파일명
      * @return bool 성공 여부
      */
     bool saveToJsonFile(const std::string& filename) const;
    
private:
    LogStatistics stats_;
    std::chrono::steady_clock::time_point startTime_;
    
    /**
     * @brief 가장 많은 로그 레벨 계산
     */
    void calculateMostCommonLevel();
}; 