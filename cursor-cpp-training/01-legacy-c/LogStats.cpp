#include "LogStats.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

// 생성자
LogStats::LogStats()
{
    reset();
}

// 통계 계산
void LogStats::calculateStatistics(const std::vector<LogEntry>& entries)
{
    startTime_ = std::chrono::steady_clock::now();
    
    // 통계 초기화
    stats_ = LogStatistics{};
    stats_.totalEntries = entries.size();
    
    // 각 엔트리별로 레벨 카운팅
    for (const auto& entry : entries)
    {
        switch (entry.level)
        {
            case LogLevel::DEBUG:
                stats_.debugCount++;
                break;
            case LogLevel::INFO:
                stats_.infoCount++;
                break;
            case LogLevel::WARN:
                stats_.warnCount++;
                break;
            case LogLevel::ERROR:
                stats_.errorCount++;
                break;
            case LogLevel::UNKNOWN:
            default:
                stats_.unknownCount++;
                break;
        }
    }
    
    // 비율 계산
    calculatePercentages();
    
    endTime_ = std::chrono::steady_clock::now();
    recordTiming();
}

// 통계 정보 반환
const LogStatistics& LogStats::getStatistics() const noexcept
{
    return stats_;
}

// 통계 출력
void LogStats::printStatistics(bool detailed) const
{
    std::cout << getStatisticsString(detailed) << std::endl;
}

// 통계 문자열 생성
std::string LogStats::getStatisticsString(bool detailed) const
{
    std::ostringstream oss;
    
    oss << "\n=== 로그 분석 결과 ===\n";
    oss << "전체 라인 수: " << stats_.totalEntries << "\n";
    oss << "ERROR 개수: " << stats_.errorCount 
        << " (" << std::fixed << std::setprecision(1) << stats_.errorPercentage << "%)\n";
    oss << "WARNING 개수: " << stats_.warnCount 
        << " (" << std::fixed << std::setprecision(1) << stats_.warnPercentage << "%)\n";
    oss << "INFO 개수: " << stats_.infoCount 
        << " (" << std::fixed << std::setprecision(1) << stats_.infoPercentage << "%)\n";
    
    if (detailed)
    {
        oss << "DEBUG 개수: " << stats_.debugCount 
            << " (" << std::fixed << std::setprecision(1) << stats_.debugPercentage << "%)\n";
        oss << "UNKNOWN 개수: " << stats_.unknownCount 
            << " (" << std::fixed << std::setprecision(1) << stats_.unknownPercentage << "%)\n";
        oss << "\n분석 소요 시간: " << std::fixed << std::setprecision(3) 
            << stats_.analysisTimeSec << "초\n";
        
        // 문제점 분석
        if (hasCriticalIssues())
        {
            oss << "\n⚠️  경고: ERROR 비율이 높습니다! (" << stats_.errorPercentage << "%)\n";
        }
        
        std::size_t issueCount = getIssueCount();
        if (issueCount > 0)
        {
            double issuePercentage = (static_cast<double>(issueCount) / stats_.totalEntries) * 100.0;
            oss << "총 문제 개수 (ERROR + WARN): " << issueCount 
                << " (" << std::fixed << std::setprecision(1) << issuePercentage << "%)\n";
        }
    }
    
    oss << "분석 완료 시간: " << stats_.analysisEndTime;
    
    return oss.str();
}

// 레벨별 개수 조회
std::size_t LogStats::getCountByLevel(LogLevel level) const noexcept
{
    switch (level)
    {
        case LogLevel::DEBUG:   return stats_.debugCount;
        case LogLevel::INFO:    return stats_.infoCount;
        case LogLevel::WARN:    return stats_.warnCount;
        case LogLevel::ERROR:   return stats_.errorCount;
        case LogLevel::UNKNOWN: return stats_.unknownCount;
        default:                return 0;
    }
}

// 레벨별 비율 조회
double LogStats::getPercentageByLevel(LogLevel level) const noexcept
{
    switch (level)
    {
        case LogLevel::DEBUG:   return stats_.debugPercentage;
        case LogLevel::INFO:    return stats_.infoPercentage;
        case LogLevel::WARN:    return stats_.warnPercentage;
        case LogLevel::ERROR:   return stats_.errorPercentage;
        case LogLevel::UNKNOWN: return stats_.unknownPercentage;
        default:                return 0.0;
    }
}

// 총 개수 반환
std::size_t LogStats::getTotalCount() const noexcept
{
    return stats_.totalEntries;
}

// 문제 개수 반환
std::size_t LogStats::getIssueCount() const noexcept
{
    return stats_.errorCount + stats_.warnCount;
}

// 심각한 문제 여부
bool LogStats::hasCriticalIssues(double threshold) const noexcept
{
    return stats_.errorPercentage > threshold;
}

// 통계 초기화
void LogStats::reset() noexcept
{
    stats_ = LogStatistics{};
    startTime_ = std::chrono::steady_clock::time_point{};
    endTime_ = std::chrono::steady_clock::time_point{};
}

// JSON 형식 출력
std::string LogStats::toJson() const
{
    std::ostringstream oss;
    
    oss << "{\n";
    oss << "  \"totalEntries\": " << stats_.totalEntries << ",\n";
    oss << "  \"counts\": {\n";
    oss << "    \"error\": " << stats_.errorCount << ",\n";
    oss << "    \"warn\": " << stats_.warnCount << ",\n";
    oss << "    \"info\": " << stats_.infoCount << ",\n";
    oss << "    \"debug\": " << stats_.debugCount << ",\n";
    oss << "    \"unknown\": " << stats_.unknownCount << "\n";
    oss << "  },\n";
    oss << "  \"percentages\": {\n";
    oss << "    \"error\": " << std::fixed << std::setprecision(2) << stats_.errorPercentage << ",\n";
    oss << "    \"warn\": " << std::fixed << std::setprecision(2) << stats_.warnPercentage << ",\n";
    oss << "    \"info\": " << std::fixed << std::setprecision(2) << stats_.infoPercentage << ",\n";
    oss << "    \"debug\": " << std::fixed << std::setprecision(2) << stats_.debugPercentage << ",\n";
    oss << "    \"unknown\": " << std::fixed << std::setprecision(2) << stats_.unknownPercentage << "\n";
    oss << "  },\n";
    oss << "  \"analysisTime\": " << std::fixed << std::setprecision(3) << stats_.analysisTimeSec << ",\n";
    oss << "  \"hasCriticalIssues\": " << (hasCriticalIssues() ? "true" : "false") << "\n";
    oss << "}";
    
    return oss.str();
}

// private: 비율 계산
void LogStats::calculatePercentages()
{
    if (stats_.totalEntries == 0)
    {
        return;
    }
    
    double total = static_cast<double>(stats_.totalEntries);
    
    stats_.debugPercentage = (static_cast<double>(stats_.debugCount) / total) * 100.0;
    stats_.infoPercentage = (static_cast<double>(stats_.infoCount) / total) * 100.0;
    stats_.warnPercentage = (static_cast<double>(stats_.warnCount) / total) * 100.0;
    stats_.errorPercentage = (static_cast<double>(stats_.errorCount) / total) * 100.0;
    stats_.unknownPercentage = (static_cast<double>(stats_.unknownCount) / total) * 100.0;
}

// private: 시간 기록
void LogStats::recordTiming()
{
    auto duration = endTime_ - startTime_;
    stats_.analysisTimeSec = std::chrono::duration<double>(duration).count();
    
    // 현재 시간을 문자열로 변환
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    stats_.analysisEndTime = oss.str();
    
    // 시작 시간도 기록 (역산)
    auto startTimeT = time_t - static_cast<std::time_t>(stats_.analysisTimeSec);
    std::ostringstream startOss;
    startOss << std::put_time(std::localtime(&startTimeT), "%Y-%m-%d %H:%M:%S");
    stats_.analysisStartTime = startOss.str();
}

// private: 시간 문자열 변환
std::string LogStats::timeToString(const std::chrono::steady_clock::time_point& timePoint) const
{
    // 이 함수는 현재 사용되지 않지만, 나중에 확장 가능
    auto duration = timePoint.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    return std::to_string(seconds.count());
} 