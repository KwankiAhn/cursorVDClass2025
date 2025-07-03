#pragma once

#include "LogFileReader.hpp"
#include "LogParser.hpp"
#include "LogStats.hpp"
#include <string>
#include <chrono>
#include <fstream>
#include <memory>
#include <atomic>
#include <thread>
#include <deque>
#include <mutex>

/**
 * @brief 실시간 로그 파일 감시 클래스
 * 파일 변경을 감지하고 새로운 로그 라인을 실시간으로 처리
 */
class LogFileWatcher
{
public:
    /**
     * @brief 생성자
     * @param logFilePath 감시할 로그 파일 경로
     */
    explicit LogFileWatcher(const std::string& logFilePath);
    
    /**
     * @brief 소멸자
     */
    ~LogFileWatcher();
    
    /**
     * @brief 실시간 감시 시작
     * @param keyword 검색할 키워드 (선택사항)
     */
    void startWatching(const std::string& keyword = "");
    
    /**
     * @brief 감시 중지
     */
    void stopWatching();
    
    /**
     * @brief 감시 상태 확인
     * @return bool 감시 중이면 true
     */
    bool isWatching() const;
    
    /**
     * @brief JSON 상태 파일 출력 여부 설정
     * @param enable JSON 출력 활성화 여부
     */
    void setJsonOutputEnabled(bool enable);

private:
    /**
     * @brief 파일 변경 감지 및 처리
     */
    void watchLoop();
    
    /**
     * @brief 파일 변경 확인
     * @return bool 변경이 있으면 true
     */
    bool checkFileChanges();
    
    /**
     * @brief 새로운 로그 라인 처리
     */
    void processNewLines();
    
    /**
     * @brief JSON 상태 파일 출력
     */
    void writeStatusJson();
    
    /**
     * @brief JSON 통계 파일 출력
     */
    void writeStatsJson();
    
    /**
     * @brief 로그 버퍼에 엔트리 추가
     * @param entry 추가할 로그 엔트리
     */
    void addToBuffer(const LogEntry& entry);

private:
    std::string logFilePath_;
    std::string keyword_;
    std::streampos lastPosition_;
    std::atomic<bool> isWatching_{false};
    std::atomic<bool> shouldStop_{false};
    bool jsonOutputEnabled_{true};
    
    std::unique_ptr<std::thread> watchThread_;
    std::unique_ptr<LogParser> parser_;
    std::unique_ptr<LogStats> stats_;
    
    // 최근 로그 버퍼 (최대 50개)
    std::deque<LogEntry> recentLogBuffer_;
    std::mutex bufferMutex_;
    static const size_t MAX_BUFFER_SIZE = 50;
    
    // 감시 간격 (밀리초)
    static const int WATCH_INTERVAL_MS = 1000;
}; 