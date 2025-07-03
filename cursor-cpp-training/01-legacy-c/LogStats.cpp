#include "LogStats.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

void LogStats::addEntry(const LogEntry& entry)
{
    stats_.totalLines++;
    stats_.levelCounts[entry.level]++;
    
    // 에러 로그 따로 저장
    if (entry.level == "ERROR")
    {
        stats_.errorEntries.push_back(entry);
    }
}

void LogStats::addEntryWithKeyword(const LogEntry& entry, const std::string& keyword)
{
    addEntry(entry);
    
    // 키워드 매칭 확인
    if (!keyword.empty())
    {
        LogParser parser;
        if (parser.containsKeyword(entry.rawLine, keyword))
        {
            stats_.keywordMatches.push_back(entry);
            stats_.keywordCounts[keyword]++;
        }
    }
}

void LogStats::finalize()
{
    calculateMostCommonLevel();
}

const LogStatistics& LogStats::getStatistics() const noexcept
{
    return stats_;
}

size_t LogStats::getCountForLevel(const std::string& level) const
{
    auto it = stats_.levelCounts.find(level);
    return (it != stats_.levelCounts.end()) ? it->second : 0;
}

double LogStats::getPercentageForLevel(const std::string& level) const
{
    if (stats_.totalLines == 0)
    {
        return 0.0;
    }
    
    size_t count = getCountForLevel(level);
    return (static_cast<double>(count) / stats_.totalLines) * 100.0;
}

std::string LogStats::getMostCommonLevel() const
{
    if (stats_.mostCommonLevel.empty())
    {
        const_cast<LogStats*>(this)->calculateMostCommonLevel();
    }
    return stats_.mostCommonLevel;
}

const std::vector<LogEntry>& LogStats::getErrorEntries() const noexcept
{
    return stats_.errorEntries;
}

const std::vector<LogEntry>& LogStats::getKeywordMatches() const noexcept
{
    return stats_.keywordMatches;
}

void LogStats::reset()
{
    stats_ = LogStatistics();
}

void LogStats::setProcessingTime(double timeMs)
{
    stats_.processingTimeMs = timeMs;
}

void LogStats::printStatistics() const
{
    std::cout << '\n';
    std::cout << "=== 📊 로그 분석 결과 ===\n";
    std::cout << "📄 전체 라인 수: " << stats_.totalLines << '\n';
    
    // 로그 레벨별 통계
    for (const auto& [level, count] : stats_.levelCounts)
    {
        if (count > 0)
        {
            const double percentage = getPercentageForLevel(level);
            std::cout << "🏷️  " << level << " 개수: " << count 
                     << " (" << std::fixed << std::setprecision(1) << percentage << "%)\n";
        }
    }
    
    // 가장 많은 로그 레벨
    if (!stats_.mostCommonLevel.empty())
    {
        std::cout << "📈 가장 많은 레벨: " << stats_.mostCommonLevel << '\n';
    }
    
    // 키워드 매칭 결과
    if (!stats_.keywordCounts.empty())
    {
        std::cout << "🔍 키워드 매칭 결과:\n";
        for (const auto& [keyword, count] : stats_.keywordCounts)
        {
            std::cout << "   '" << keyword << "': " << count << "개\n";
        }
    }
    
    // 에러 로그 개수
    if (!stats_.errorEntries.empty())
    {
        std::cout << "❌ 에러 로그 개수: " << stats_.errorEntries.size() << "개\n";
    }
    
    // 처리 시간
    if (stats_.processingTimeMs > 0.0)
    {
        std::cout << "⏱️  처리 시간: " << std::fixed << std::setprecision(2) 
                 << stats_.processingTimeMs << "ms\n";
    }
    
    std::cout << "✅ 분석 완료!\n";
}

void LogStats::calculateMostCommonLevel()
{
    size_t maxCount = 0;
    std::string mostCommon;
    
    for (const auto& [level, count] : stats_.levelCounts)
    {
        if (count > maxCount)
        {
            maxCount = count;
            mostCommon = level;
        }
    }
    
    stats_.mostCommonLevel = mostCommon;
}

std::string LogStats::toJson() const
{
    using json = nlohmann::json;
    
    json result;
    
    // 요약 정보
    result["summary"] = {
        {"totalLines", stats_.totalLines},
        {"mostCommonLevel", stats_.mostCommonLevel},
        {"processingTimeMs", stats_.processingTimeMs}
    };
    
    // 로그 레벨별 통계
    result["levelCounts"] = json::object();
    for (const auto& [level, count] : stats_.levelCounts)
    {
        if (count > 0)
        {
            result["levelCounts"][level] = {
                {"count", count},
                {"percentage", getPercentageForLevel(level)}
            };
        }
    }
    
    // 키워드 매칭 통계
    result["keywordCounts"] = json::object();
    for (const auto& [keyword, count] : stats_.keywordCounts)
    {
        result["keywordCounts"][keyword] = count;
    }
    
    // 에러 로그 엔트리들
    result["errorEntries"] = json::array();
    for (const auto& entry : stats_.errorEntries)
    {
        result["errorEntries"].push_back({
            {"timestamp", entry.timestamp},
            {"level", entry.level},
            {"message", entry.message},
            {"rawLine", entry.rawLine}
        });
    }
    
    // 키워드 매칭 엔트리들 (최대 10개만)
    result["keywordMatches"] = json::array();
    size_t maxKeywordMatches = std::min(stats_.keywordMatches.size(), size_t(10));
    for (size_t i = 0; i < maxKeywordMatches; ++i)
    {
        const auto& entry = stats_.keywordMatches[i];
        result["keywordMatches"].push_back({
            {"timestamp", entry.timestamp},
            {"level", entry.level},
            {"message", entry.message},
            {"rawLine", entry.rawLine}
        });
    }
    
    // 4칸 들여쓰기로 예쁘게 포맷팅
    return result.dump(4);
}

bool LogStats::saveToJsonFile(const std::string& filename) const
{
    try
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "JSON 파일을 열 수 없습니다: " << filename << std::endl;
            return false;
        }
        
        file << toJson();
        file.close();
        
        std::cout << "📄 JSON 결과가 저장되었습니다: " << filename << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "JSON 파일 저장 중 오류 발생: " << e.what() << std::endl;
        return false;
    }
} 