#pragma once

#include "LogParser.hpp"
#include <unordered_map>
#include <chrono>
#include <string>

namespace LogAnalyzer {

struct Statistics {
    std::size_t totalLines = 0;
    std::unordered_map<LogLevel, std::size_t> levelCounts;
    std::chrono::system_clock::time_point analysisTime;
    std::string filePath;
    std::uintmax_t fileSize = 0;
    
    Statistics() : analysisTime(std::chrono::system_clock::now()) {}
};

class LogStats {
public:
    LogStats() = default;
    ~LogStats() = default;

    // 통계 계산
    Statistics calculateStats(const std::vector<LogEntry>& entries, 
                            const std::string& filePath = "", 
                            std::uintmax_t fileSize = 0);
    
    // 통계 출력
    void printStats(const Statistics& stats) const;
    void printDetailedStats(const Statistics& stats) const;
    
    // JSON 형태로 통계 출력
    std::string statsToJson(const Statistics& stats) const;
    
    // 특정 레벨의 엔트리들 출력
    void printEntriesByLevel(const std::vector<LogEntry>& entries, LogLevel level) const;
    
    // 키워드 매칭 엔트리들 출력
    void printKeywordMatches(const std::vector<LogEntry>& entries, 
                           const std::string& keyword) const;

private:
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timePoint) const;
    std::string formatFileSize(std::uintmax_t size) const;
    double calculatePercentage(std::size_t count, std::size_t total) const;
};

} // namespace LogAnalyzer 