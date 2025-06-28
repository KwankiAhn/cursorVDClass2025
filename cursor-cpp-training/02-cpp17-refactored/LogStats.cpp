#include "LogStats.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace LogAnalyzer {

Statistics LogStats::calculateStats(const std::vector<LogEntry>& entries, 
                                   const std::string& filePath, 
                                   std::uintmax_t fileSize) {
    Statistics stats;
    stats.totalLines = entries.size();
    stats.filePath = filePath;
    stats.fileSize = fileSize;
    
    // 로그 레벨별 카운트
    for (const auto& entry : entries) {
        stats.levelCounts[entry.level]++;
    }
    
    return stats;
}

void LogStats::printStats(const Statistics& stats) const {
    std::cout << "\n=== 로그 분석 결과 ===\n";
    std::cout << "파일 경로: " << stats.filePath << "\n";
    std::cout << "파일 크기: " << formatFileSize(stats.fileSize) << "\n";
    std::cout << "전체 라인 수: " << stats.totalLines << "\n";
    
    for (const auto& [level, count] : stats.levelCounts) {
        if (count > 0) {
            std::cout << LogParser::logLevelToString(level) << " 개수: " 
                     << count << " (" << std::fixed << std::setprecision(1) 
                     << calculatePercentage(count, stats.totalLines) << "%)\n";
        }
    }
    
    std::cout << "분석 완료 시간: " << formatTimestamp(stats.analysisTime) << "\n";
}

void LogStats::printDetailedStats(const Statistics& stats) const {
    printStats(stats);
    
    std::cout << "\n=== 상세 통계 ===\n";
    
    // 로그 레벨별 상세 정보
    std::cout << std::left << std::setw(12) << "로그 레벨" 
              << std::setw(10) << "개수" 
              << std::setw(10) << "비율(%)" 
              << "막대 그래프\n";
    std::cout << std::string(50, '-') << "\n";
    
    for (int i = 0; i <= static_cast<int>(LogLevel::DEBUG); ++i) {
        LogLevel level = static_cast<LogLevel>(i);
        auto it = stats.levelCounts.find(level);
        std::size_t count = (it != stats.levelCounts.end()) ? it->second : 0;
        
        if (count > 0 || level == LogLevel::ERROR || level == LogLevel::WARNING || level == LogLevel::INFO) {
            double percentage = calculatePercentage(count, stats.totalLines);
            int barLength = static_cast<int>(percentage / 2); // 50% = 25 characters
            
            std::cout << std::left << std::setw(12) << LogParser::logLevelToString(level)
                     << std::setw(10) << count
                     << std::setw(10) << std::fixed << std::setprecision(1) << percentage;
            
            for(int j = 0; j < barLength; ++j) {
                std::cout << "█";
            }
            std::cout << "\n";
        }
    }
}

std::string LogStats::statsToJson(const Statistics& stats) const {
    std::ostringstream json;
    json << "{\n";
    json << "  \"filePath\": \"" << stats.filePath << "\",\n";
    json << "  \"fileSize\": " << stats.fileSize << ",\n";
    json << "  \"totalLines\": " << stats.totalLines << ",\n";
    json << "  \"analysisTime\": \"" << formatTimestamp(stats.analysisTime) << "\",\n";
    json << "  \"levelCounts\": {\n";
    
    bool first = true;
    for (const auto& [level, count] : stats.levelCounts) {
        if (!first) json << ",\n";
        json << "    \"" << LogParser::logLevelToString(level) << "\": " << count;
        first = false;
    }
    
    json << "\n  }\n";
    json << "}";
    
    return json.str();
}

void LogStats::printEntriesByLevel(const std::vector<LogEntry>& entries, LogLevel level) const {
    std::cout << "\n=== " << LogParser::logLevelToString(level) << " 로그 엔트리 ===\n";
    
    std::size_t count = 0;
    for (const auto& entry : entries) {
        if (entry.level == level) {
            std::cout << "[" << ++count << "] " << entry.originalLine << "\n";
        }
    }
    
    if (count == 0) {
        std::cout << "해당 레벨의 로그가 없습니다.\n";
    } else {
        std::cout << "총 " << count << "개의 " << LogParser::logLevelToString(level) << " 로그를 발견했습니다.\n";
    }
}

void LogStats::printKeywordMatches(const std::vector<LogEntry>& entries, 
                                 const std::string& keyword) const {
    std::cout << "\n=== 키워드 '" << keyword << "' 검색 결과 ===\n";
    
    std::size_t count = 0;
    for (const auto& entry : entries) {
        if (entry.originalLine.find(keyword) != std::string::npos) {
            std::cout << "[" << ++count << "] [" << LogParser::logLevelToString(entry.level) << "] " 
                     << entry.originalLine << "\n";
        }
    }
    
    if (count == 0) {
        std::cout << "키워드를 포함한 로그가 없습니다.\n";
    } else {
        std::cout << "총 " << count << "개의 매칭 로그를 발견했습니다.\n";
    }
}

std::string LogStats::formatTimestamp(const std::chrono::system_clock::time_point& timePoint) const {
    auto time_t = std::chrono::system_clock::to_time_t(timePoint);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string LogStats::formatFileSize(std::uintmax_t size) const {
    const char* units[] = {"B", "KB", "MB", "GB"};
    double fileSize = static_cast<double>(size);
    int unitIndex = 0;
    
    while (fileSize >= 1024.0 && unitIndex < 3) {
        fileSize /= 1024.0;
        unitIndex++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << fileSize << " " << units[unitIndex];
    return oss.str();
}

double LogStats::calculatePercentage(std::size_t count, std::size_t total) const {
    if (total == 0) return 0.0;
    return (static_cast<double>(count) / static_cast<double>(total)) * 100.0;
}

} // namespace LogAnalyzer 