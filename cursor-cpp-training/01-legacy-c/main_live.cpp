#include "LogFileWatcher.hpp"
#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

// 전역 변수로 감시자 포인터 저장
std::unique_ptr<LogFileWatcher> g_watcher = nullptr;
std::atomic<bool> g_running{true};

/**
 * @brief 시그널 핸들러 (Ctrl+C 처리)
 * @param signal 시그널 번호
 */
void signalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        std::cout << "\n🛑 종료 신호 수신됨..." << std::endl;
        g_running.store(false);
        
        if (g_watcher)
        {
            g_watcher->stopWatching();
        }
    }
}

/**
 * @brief 사용법 출력
 * @param programName 프로그램 이름
 */
void printUsage(const char* programName)
{
    std::cout << "=== 실시간 로그 모니터링 시스템 ===" << std::endl;
    std::cout << "사용법: " << programName << " <로그파일> [검색키워드]" << std::endl;
    std::cout << std::endl;
    std::cout << "예시:" << std::endl;
    std::cout << "  " << programName << " sample.log" << std::endl;
    std::cout << "  " << programName << " sample.log ERROR" << std::endl;
    std::cout << "  " << programName << " /var/log/system.log database" << std::endl;
    std::cout << std::endl;
    std::cout << "기능:" << std::endl;
    std::cout << "  - 실시간 로그 파일 감시" << std::endl;
    std::cout << "  - ERROR 발생 시 즉시 알림" << std::endl;
    std::cout << "  - 키워드 검색 (선택사항)" << std::endl;
    std::cout << "  - JSON 파일 생성 (status.json, stats.json)" << std::endl;
    std::cout << std::endl;
    std::cout << "종료: Ctrl+C" << std::endl;
}

int main(int argc, char* argv[])
{
    try
    {
        // 인자 확인
        if (argc < 2)
        {
            printUsage(argv[0]);
            return 1;
        }
        
        std::string logFilePath = argv[1];
        std::string keyword = (argc >= 3) ? argv[2] : "";
        
        // 시그널 핸들러 등록
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        std::cout << "🚀 실시간 로그 모니터링 시스템 시작" << std::endl;
        std::cout << "📁 대상 파일: " << logFilePath << std::endl;
        
        if (!keyword.empty())
        {
            std::cout << "🔎 검색 키워드: " << keyword << std::endl;
        }
        
        std::cout << "📄 JSON 출력: status.json, stats.json" << std::endl;
        std::cout << "⏹️  종료하려면 Ctrl+C를 누르세요" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        // LogFileWatcher 생성 및 시작
        g_watcher = std::make_unique<LogFileWatcher>(logFilePath);
        g_watcher->setJsonOutputEnabled(true);
        g_watcher->startWatching(keyword);
        
        // 메인 루프 (사용자 입력 또는 종료 신호 대기)
        while (g_running.load() && g_watcher->isWatching())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // 정리
        if (g_watcher)
        {
            g_watcher->stopWatching();
            g_watcher.reset();
        }
        
        std::cout << "🏁 프로그램 종료" << std::endl;
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "❌ 오류 발생: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "❌ 알 수 없는 오류가 발생했습니다." << std::endl;
        return 1;
    }
} 