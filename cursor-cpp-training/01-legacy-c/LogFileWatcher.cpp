#include "LogFileWatcher.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>

// static 멤버 변수 정의
const size_t LogFileWatcher::MAX_BUFFER_SIZE;
const int LogFileWatcher::WATCH_INTERVAL_MS;

LogFileWatcher::LogFileWatcher(const std::string& logFilePath)
    : logFilePath_(logFilePath)
    , lastPosition_(0)
    , parser_(std::make_unique<LogParser>())
    , stats_(std::make_unique<LogStats>())
{
    // 파일 존재 여부 확인
    if (!std::filesystem::exists(logFilePath_))
    {
        throw std::runtime_error("로그 파일이 존재하지 않습니다: " + logFilePath_);
    }
    
    // 초기 파일 크기 확인
    std::ifstream file(logFilePath_, std::ios::ate);
    if (file.is_open())
    {
        lastPosition_ = file.tellg();
        file.close();
    }
    
    std::cout << "📁 로그 파일 감시 준비 완료: " << logFilePath_ << std::endl;
}

LogFileWatcher::~LogFileWatcher()
{
    stopWatching();
}

void LogFileWatcher::startWatching(const std::string& keyword)
{
    if (isWatching_.load())
    {
        std::cout << "⚠️  이미 감시 중입니다." << std::endl;
        return;
    }
    
    keyword_ = keyword;
    shouldStop_.store(false);
    isWatching_.store(true);
    
    std::cout << "🔍 실시간 로그 감시 시작..." << std::endl;
    if (!keyword_.empty())
    {
        std::cout << "🔎 검색 키워드: " << keyword_ << std::endl;
    }
    
    // 감시 스레드 시작
    watchThread_ = std::make_unique<std::thread>(&LogFileWatcher::watchLoop, this);
}

void LogFileWatcher::stopWatching()
{
    if (!isWatching_.load())
    {
        return;
    }
    
    std::cout << "🛑 로그 감시 중지 중..." << std::endl;
    shouldStop_.store(true);
    
    if (watchThread_ && watchThread_->joinable())
    {
        watchThread_->join();
    }
    
    isWatching_.store(false);
    std::cout << "✅ 로그 감시 중지 완료" << std::endl;
}

bool LogFileWatcher::isWatching() const
{
    return isWatching_.load();
}

void LogFileWatcher::setJsonOutputEnabled(bool enable)
{
    jsonOutputEnabled_ = enable;
}

void LogFileWatcher::watchLoop()
{
    while (!shouldStop_.load())
    {
        try
        {
            if (checkFileChanges())
            {
                processNewLines();
                
                if (jsonOutputEnabled_)
                {
                    writeStatusJson();
                    writeStatsJson();
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "❌ 감시 중 오류 발생: " << e.what() << std::endl;
        }
        
        // 1초 대기
        std::this_thread::sleep_for(std::chrono::milliseconds(WATCH_INTERVAL_MS));
    }
}

bool LogFileWatcher::checkFileChanges()
{
    try
    {
        // 파일 크기 확인
        auto currentSize = std::filesystem::file_size(logFilePath_);
        
        if (currentSize > static_cast<std::uintmax_t>(lastPosition_))
        {
            return true;
        }
        else if (currentSize < static_cast<std::uintmax_t>(lastPosition_))
        {
            // 파일이 회전되었을 가능성 (작아졌을 때)
            std::cout << "🔄 파일 회전 감지됨" << std::endl;
            lastPosition_ = 0;
            return true;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "❌ 파일 크기 확인 중 오류: " << e.what() << std::endl;
    }
    
    return false;
}

void LogFileWatcher::processNewLines()
{
    std::ifstream file(logFilePath_);
    if (!file.is_open())
    {
        std::cerr << "❌ 파일을 열 수 없습니다: " << logFilePath_ << std::endl;
        return;
    }
    
    // 마지막 읽은 위치로 이동
    file.seekg(lastPosition_);
    
    std::string line;
    bool hasNewError = false;
    
    while (std::getline(file, line))
    {
        if (line.empty()) continue;
        
        // 로그 파싱
        LogEntry entry = parser_->parseLine(line);
        
        // 통계 업데이트
        if (!keyword_.empty())
        {
            stats_->addEntryWithKeyword(entry, keyword_);
        }
        else
        {
            stats_->addEntry(entry);
        }
        
        // 버퍼에 추가
        addToBuffer(entry);
        
        // ERROR 로그 처리
        if (entry.level == "ERROR")
        {
            hasNewError = true;
            std::cout << "🚨 [ERROR 감지] " << entry.rawLine << std::endl;
        }
        
        // 키워드 매칭 확인
        if (!keyword_.empty() && parser_->containsKeyword(entry.rawLine, keyword_))
        {
            std::cout << "🔍 [키워드 발견] " << entry.rawLine << std::endl;
        }
    }
    
    // 현재 위치 업데이트
    lastPosition_ = file.tellg();
    file.close();
    
    if (hasNewError)
    {
        std::cout << "🔔 새로운 에러 발생 알림!" << std::endl;
    }
}

void LogFileWatcher::writeStatusJson()
{
    using json = nlohmann::json;
    
    const auto& statistics = stats_->getStatistics();
    
    json statusJson;
    statusJson["hasNewError"] = (statistics.errorEntries.size() > 0);
    statusJson["errorCount"] = statistics.levelCounts.at("ERROR");
    statusJson["totalLines"] = statistics.totalLines;
    statusJson["lastUpdate"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // 키워드 관련 정보
    if (!keyword_.empty())
    {
        statusJson["keyword"] = keyword_;
        auto keywordIt = statistics.keywordCounts.find(keyword_);
        statusJson["keywordCount"] = (keywordIt != statistics.keywordCounts.end()) ? keywordIt->second : 0;
    }
    
    // status.json 파일 쓰기
    std::ofstream file("status.json");
    if (file.is_open())
    {
        file << statusJson.dump(2);
        file.close();
    }
    else
    {
        std::cerr << "❌ status.json 파일 쓰기 실패" << std::endl;
    }
}

void LogFileWatcher::writeStatsJson()
{
    using json = nlohmann::json;
    
    const auto& statistics = stats_->getStatistics();
    
    json statsJson;
    
    // 기본 통계
    statsJson["summary"] = {
        {"totalLines", statistics.totalLines},
        {"lastUpdate", std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()}
    };
    
    // 로그 레벨별 통계
    statsJson["levelCounts"] = json::object();
    for (const auto& [level, count] : statistics.levelCounts)
    {
        if (count > 0)
        {
            statsJson["levelCounts"][level] = count;
        }
    }
    
    // 최근 로그 버퍼 (최대 20개만)
    {
        std::lock_guard<std::mutex> lock(bufferMutex_);
        statsJson["recentLogs"] = json::array();
        
        size_t startIndex = recentLogBuffer_.size() > 20 ? recentLogBuffer_.size() - 20 : 0;
        for (size_t i = startIndex; i < recentLogBuffer_.size(); ++i)
        {
            const auto& entry = recentLogBuffer_[i];
            statsJson["recentLogs"].push_back({
                {"timestamp", entry.timestamp},
                {"level", entry.level},
                {"message", entry.message},
                {"rawLine", entry.rawLine}
            });
        }
    }
    
    // 키워드 매칭 정보
    if (!keyword_.empty())
    {
        statsJson["keywordMatches"] = json::array();
        for (const auto& entry : statistics.keywordMatches)
        {
            statsJson["keywordMatches"].push_back({
                {"timestamp", entry.timestamp},
                {"level", entry.level},
                {"message", entry.message},
                {"rawLine", entry.rawLine}
            });
        }
    }
    
    // stats.json 파일 쓰기
    std::ofstream file("stats.json");
    if (file.is_open())
    {
        file << statsJson.dump(2);
        file.close();
    }
    else
    {
        std::cerr << "❌ stats.json 파일 쓰기 실패" << std::endl;
    }
}

void LogFileWatcher::addToBuffer(const LogEntry& entry)
{
    std::lock_guard<std::mutex> lock(bufferMutex_);
    
    if (recentLogBuffer_.size() >= MAX_BUFFER_SIZE)
    {
        recentLogBuffer_.pop_front();
    }
    
    recentLogBuffer_.push_back(entry);
} 