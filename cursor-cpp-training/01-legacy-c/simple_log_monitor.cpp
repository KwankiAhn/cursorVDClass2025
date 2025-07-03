#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <filesystem>
#include <regex>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>

class SimpleLogMonitor
{
private:
    std::string logFilePath_;
    std::streampos lastPosition_;
    int errorCount_ = 0;
    int totalLines_ = 0;
    std::string lastError_;
    
public:
    SimpleLogMonitor(const std::string& logFile) : logFilePath_(logFile), lastPosition_(0) {
        // 초기 파일 크기 확인
        std::ifstream file(logFilePath_, std::ios::ate);
        if (file.is_open()) {
            lastPosition_ = file.tellg();
            file.close();
        }
    }
    
    void startMonitoring() {
        std::cout << "🔍 간단한 로그 모니터링 시작: " << logFilePath_ << std::endl;
        
        while (true) {
            checkForChanges();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
private:
    void checkForChanges() {
        try {
            auto currentSize = std::filesystem::file_size(logFilePath_);
            
            if (currentSize > static_cast<std::uintmax_t>(lastPosition_)) {
                processNewLines();
                writeJsonFiles();
            }
        } catch (const std::exception& e) {
            std::cerr << "파일 확인 오류: " << e.what() << std::endl;
        }
    }
    
    void processNewLines() {
        std::ifstream file(logFilePath_);
        if (!file.is_open()) return;
        
        file.seekg(lastPosition_);
        std::string line;
        
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            totalLines_++;
            
            // ERROR 감지 (대소문자 무관)
            std::string upperLine = line;
            std::transform(upperLine.begin(), upperLine.end(), upperLine.begin(), ::toupper);
            
            if (upperLine.find("ERROR") != std::string::npos) {
                errorCount_++;
                lastError_ = line;
                std::cout << "🚨 [ERROR 감지] " << line << std::endl;
            }
            
            std::cout << "📝 [새 로그] " << line << std::endl;
        }
        
        lastPosition_ = file.tellg();
        file.close();
    }
    
    void writeJsonFiles() {
        using json = nlohmann::json;
        
        // 현재 시간을 문자열로 변환
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream timeStream;
        timeStream << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        std::string timeString = timeStream.str();
        
        // status.json 생성
        json status;
        status["hasNewError"] = !lastError_.empty();
        status["errorCount"] = errorCount_;
        status["totalLines"] = totalLines_;
        status["lastUpdate"] = timeString;
        status["lastError"] = lastError_;
        
        std::ofstream statusFile("status.json");
        if (statusFile.is_open()) {
            statusFile << status.dump(2);
            statusFile.close();
        }
        
        // stats.json 생성  
        json stats;
        stats["summary"]["totalLines"] = totalLines_;
        stats["summary"]["errorCount"] = errorCount_;
        stats["summary"]["lastUpdate"] = timeString;
        
        // 간단한 최근 로그 (마지막 에러만)
        if (!lastError_.empty()) {
            stats["recentLogs"] = json::array();
            json logEntry;
            logEntry["level"] = "ERROR";
            logEntry["message"] = lastError_;
            logEntry["timestamp"] = timeString;
            stats["recentLogs"].push_back(logEntry);
        }
        
        std::ofstream statsFile("stats.json");
        if (statsFile.is_open()) {
            statsFile << stats.dump(2);
            statsFile.close();
        }
        
        std::cout << "📄 JSON 파일 업데이트됨 (status.json, stats.json)" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "사용법: " << argv[0] << " <로그파일>" << std::endl;
        return 1;
    }
    
    try {
        SimpleLogMonitor monitor(argv[1]);
        monitor.startMonitoring();
    } catch (const std::exception& e) {
        std::cerr << "오류: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 