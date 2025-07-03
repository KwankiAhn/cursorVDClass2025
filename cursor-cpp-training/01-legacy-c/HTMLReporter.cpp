#include "HTMLReporter.hpp"
#include <iostream>
#include <filesystem>

HTMLReporter::HTMLReporter(const std::string& outputPath)
    : outputPath_(outputPath)
{
}

bool HTMLReporter::generateReport(const LogStats& stats, const std::string& logFilePath)
{
    try
    {
        std::ofstream file(outputPath_);
        if (!file.is_open())
        {
            std::cerr << "❌ HTML 파일을 열 수 없습니다: " << outputPath_ << std::endl;
            return false;
        }
        
        std::string htmlContent = generateHTMLContent(stats, logFilePath);
        file << htmlContent;
        file.close();
        
        std::cout << "✅ HTML 리포트 생성 완료: " << outputPath_ << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "❌ HTML 리포트 생성 중 오류 발생: " << e.what() << std::endl;
        return false;
    }
}

std::string HTMLReporter::generateHTMLContent(const LogStats& stats, const std::string& logFilePath)
{
    const auto& statistics = stats.getStatistics();
    
    std::ostringstream html;
    html << "<!DOCTYPE html>\n"
         << "<html lang=\"ko\">\n"
         << "<head>\n"
         << "    <meta charset=\"UTF-8\">\n"
         << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
         << "    <title>로그 모니터링 리포트</title>\n"
         << "    <style>\n"
         << generateCSS()
         << "    </style>\n"
         << "</head>\n"
         << "<body>\n"
         << "    <div class=\"container\">\n"
         << "        <div class=\"header\">\n"
         << "            <h1>🔍 로그 모니터링 리포트</h1>\n"
         << "            <div class=\"timestamp\">마지막 업데이트: " << getCurrentTime() << "</div>\n"
         << "            <div class=\"file-info\">📁 분석 파일: " << logFilePath << "</div>\n"
         << "        </div>\n"
         << "\n"
         << "        <div class=\"refresh-notice\">\n"
         << "            💡 최신 정보를 보려면 <kbd>F5</kbd> 키를 눌러 새로고침하세요\n"
         << "            <div class=\"auto-refresh-toggle\">\n"
         << "                <button onclick=\"toggleAutoRefresh()\" id=\"autoRefreshBtn\">자동 새로고침 ON</button>\n"
         << "                <span id=\"countdown\"></span>\n"
         << "            </div>\n"
         << "        </div>\n"
         << "\n"
         << generateStatsCards(stats)
         << "\n"
         << "        <div class=\"error-section\">\n"
         << "            <h2>🚨 최근 ERROR 로그</h2>\n"
         << generateErrorList(statistics.errorEntries)
         << "        </div>\n"
         << "\n"
         << "        <div class=\"footer\">\n"
         << "            <p>처리 시간: " << std::fixed << std::setprecision(2) << statistics.processingTimeMs << "ms</p>\n"
         << "            <p>생성 시간: " << getCurrentTime() << "</p>\n"
         << "        </div>\n"
         << "    </div>\n"
         << "\n"
         << "    <script>\n"
         << "        let autoRefreshEnabled = true;\n"
         << "        let countdownInterval;\n"
         << "        let refreshInterval = 30; // 30초\n"
         << "        let currentCountdown = refreshInterval;\n"
         << "\n"
         << "        function toggleAutoRefresh() {\n"
         << "            autoRefreshEnabled = !autoRefreshEnabled;\n"
         << "            const btn = document.getElementById('autoRefreshBtn');\n"
         << "            const countdown = document.getElementById('countdown');\n"
         << "            \n"
         << "            if (autoRefreshEnabled) {\n"
         << "                btn.textContent = '자동 새로고침 ON';\n"
         << "                btn.classList.remove('disabled');\n"
         << "                startCountdown();\n"
         << "            } else {\n"
         << "                btn.textContent = '자동 새로고침 OFF';\n"
         << "                btn.classList.add('disabled');\n"
         << "                clearInterval(countdownInterval);\n"
         << "                countdown.textContent = '';\n"
         << "            }\n"
         << "        }\n"
         << "\n"
         << "        function startCountdown() {\n"
         << "            currentCountdown = refreshInterval;\n"
         << "            updateCountdown();\n"
         << "            \n"
         << "            countdownInterval = setInterval(() => {\n"
         << "                currentCountdown--;\n"
         << "                updateCountdown();\n"
         << "                \n"
         << "                if (currentCountdown <= 0) {\n"
         << "                    location.reload();\n"
         << "                }\n"
         << "            }, 1000);\n"
         << "        }\n"
         << "\n"
         << "        function updateCountdown() {\n"
         << "            const countdown = document.getElementById('countdown');\n"
         << "            countdown.textContent = ` (${currentCountdown}초 후 새로고침)`;\n"
         << "        }\n"
         << "\n"
         << "        // 페이지 로드 시 자동 시작\n"
         << "        document.addEventListener('DOMContentLoaded', function() {\n"
         << "            if (autoRefreshEnabled) {\n"
         << "                startCountdown();\n"
         << "            }\n"
         << "        });\n"
         << "\n"
         << "        // 페이지 가시성 변경 시 처리\n"
         << "        document.addEventListener('visibilitychange', function() {\n"
         << "            if (document.hidden) {\n"
         << "                clearInterval(countdownInterval);\n"
         << "            } else if (autoRefreshEnabled) {\n"
         << "                startCountdown();\n"
         << "            }\n"
         << "        });\n"
         << "    </script>\n"
         << "</body>\n"
         << "</html>";
    
    return html.str();
}

std::string HTMLReporter::getCurrentTime()
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string HTMLReporter::generateErrorList(const std::vector<LogEntry>& errorEntries)
{
    std::ostringstream html;
    
    if (errorEntries.empty())
    {
        html << "            <div class=\"no-errors\">\n"
             << "                <p>✅ ERROR 로그가 없습니다!</p>\n"
             << "            </div>\n";
        return html.str();
    }
    
    html << "            <div class=\"error-list\">\n";
    
    // 최근 10개만 표시
    size_t maxErrors = std::min(errorEntries.size(), static_cast<size_t>(10));
    for (size_t i = 0; i < maxErrors; ++i)
    {
        const auto& entry = errorEntries[i];
        html << "                <div class=\"error-item\">\n"
             << "                    <div class=\"error-time\">" << entry.timestamp << "</div>\n"
             << "                    <div class=\"error-message\">" << entry.message << "</div>\n"
             << "                    <div class=\"error-raw\">" << entry.rawLine << "</div>\n"
             << "                </div>\n";
    }
    
    if (errorEntries.size() > 10)
    {
        html << "                <div class=\"more-errors\">\n"
             << "                    <p>⋯ 총 " << errorEntries.size() << "개의 ERROR 중 최근 10개만 표시</p>\n"
             << "                </div>\n";
    }
    
    html << "            </div>\n";
    return html.str();
}

std::string HTMLReporter::generateStatsCards(const LogStats& stats)
{
    const auto& statistics = stats.getStatistics();
    
    std::ostringstream html;
    html << "        <div class=\"stats-grid\">\n"
         << "            <div class=\"stat-card total\">\n"
         << "                <div class=\"stat-icon\">📊</div>\n"
         << "                <div class=\"stat-number\">" << statistics.totalLines << "</div>\n"
         << "                <div class=\"stat-label\">총 로그 수</div>\n"
         << "            </div>\n"
         << "            <div class=\"stat-card info\">\n"
         << "                <div class=\"stat-icon\">ℹ️</div>\n"
         << "                <div class=\"stat-number\">" << stats.getCountForLevel("INFO") << "</div>\n"
         << "                <div class=\"stat-label\">INFO</div>\n"
         << "                <div class=\"stat-percent\">" << std::fixed << std::setprecision(1) << stats.getPercentageForLevel("INFO") << "%</div>\n"
         << "                <div class=\"progress-bar\">\n"
         << "                    <div class=\"progress-fill info\" style=\"width: " << stats.getPercentageForLevel("INFO") << "%\"></div>\n"
         << "                </div>\n"
         << "            </div>\n"
         << "            <div class=\"stat-card warn\">\n"
         << "                <div class=\"stat-icon\">⚠️</div>\n"
         << "                <div class=\"stat-number\">" << stats.getCountForLevel("WARNING") << "</div>\n"
         << "                <div class=\"stat-label\">WARNING</div>\n"
         << "                <div class=\"stat-percent\">" << std::fixed << std::setprecision(1) << stats.getPercentageForLevel("WARNING") << "%</div>\n"
         << "                <div class=\"progress-bar\">\n"
         << "                    <div class=\"progress-fill warn\" style=\"width: " << stats.getPercentageForLevel("WARNING") << "%\"></div>\n"
         << "                </div>\n"
         << "            </div>\n"
         << "            <div class=\"stat-card error\">\n"
         << "                <div class=\"stat-icon\">❌</div>\n"
         << "                <div class=\"stat-number\">" << stats.getCountForLevel("ERROR") << "</div>\n"
         << "                <div class=\"stat-label\">ERROR</div>\n"
         << "                <div class=\"stat-percent\">" << std::fixed << std::setprecision(1) << stats.getPercentageForLevel("ERROR") << "%</div>\n"
         << "                <div class=\"progress-bar\">\n"
         << "                    <div class=\"progress-fill error\" style=\"width: " << stats.getPercentageForLevel("ERROR") << "%\"></div>\n"
         << "                </div>\n"
         << "            </div>\n"
         << "        </div>\n";
    
    return html.str();
}

std::string HTMLReporter::generateCSS()
{
    return R"(
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
            color: #333;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            border-radius: 15px;
            box-shadow: 0 20px 40px rgba(0,0,0,0.1);
            overflow: hidden;
        }
        
        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }
        
        .header h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        
        .timestamp {
            font-size: 1.1em;
            opacity: 0.9;
            margin-bottom: 5px;
        }
        
        .file-info {
            font-size: 0.9em;
            opacity: 0.8;
            font-family: monospace;
        }
        
        .refresh-notice {
            background: #e8f5e8;
            border: 1px solid #4caf50;
            border-radius: 8px;
            padding: 15px;
            margin: 20px;
            text-align: center;
            color: #2e7d32;
            font-weight: 500;
        }
        
        .refresh-notice kbd {
            background: #2e7d32;
            color: white;
            padding: 2px 6px;
            border-radius: 3px;
            font-size: 0.9em;
        }
        
        .auto-refresh-toggle {
            float: right;
            margin-top: -5px;
        }
        
        .auto-refresh-toggle button {
            background: #4caf50;
            color: white;
            border: none;
            padding: 5px 10px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 0.8em;
        }
        
        .auto-refresh-toggle button:hover {
            background: #45a049;
        }
        
        .auto-refresh-toggle button.disabled {
            background: #666;
        }
        
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            padding: 30px;
        }
        
        .stat-card {
            background: white;
            border-radius: 10px;
            padding: 25px;
            text-align: center;
            box-shadow: 0 5px 15px rgba(0,0,0,0.1);
            transition: transform 0.3s ease;
        }
        
        .stat-card:hover {
            transform: translateY(-5px);
        }
        
        .stat-card.total {
            border-left: 5px solid #2196F3;
        }
        
        .stat-card.info {
            border-left: 5px solid #4CAF50;
        }
        
        .stat-card.warn {
            border-left: 5px solid #FF9800;
        }
        
        .stat-card.error {
            border-left: 5px solid #F44336;
        }
        
        .stat-icon {
            font-size: 2em;
            margin-bottom: 10px;
            display: block;
        }
        
        .stat-number {
            font-size: 2.5em;
            font-weight: bold;
            margin-bottom: 10px;
            animation: countUp 0.8s ease-out;
        }
        
        @keyframes countUp {
            from {
                opacity: 0;
                transform: translateY(20px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }
        
        .stat-card.total .stat-number {
            color: #2196F3;
        }
        
        .stat-card.info .stat-number {
            color: #4CAF50;
        }
        
        .stat-card.warn .stat-number {
            color: #FF9800;
        }
        
        .stat-card.error .stat-number {
            color: #F44336;
        }
        
        .stat-label {
            font-size: 1.2em;
            font-weight: 600;
            margin-bottom: 5px;
        }
        
        .stat-percent {
            font-size: 0.9em;
            color: #666;
            margin-bottom: 10px;
        }
        
        .progress-bar {
            width: 100%;
            height: 8px;
            background: #e0e0e0;
            border-radius: 4px;
            overflow: hidden;
            margin-top: 5px;
        }
        
        .progress-fill {
            height: 100%;
            border-radius: 4px;
            transition: width 1s ease-in-out;
            animation: progressAnimation 1.5s ease-in-out;
        }
        
        .progress-fill.info {
            background: linear-gradient(90deg, #4CAF50, #81C784);
        }
        
        .progress-fill.warn {
            background: linear-gradient(90deg, #FF9800, #FFB74D);
        }
        
        .progress-fill.error {
            background: linear-gradient(90deg, #F44336, #E57373);
        }
        
        @keyframes progressAnimation {
            from {
                width: 0%;
            }
        }
        
        .error-section {
            padding: 0 30px 30px;
        }
        
        .error-section h2 {
            color: #F44336;
            margin-bottom: 20px;
            font-size: 1.8em;
        }
        
        .error-list {
            space-y: 15px;
        }
        
        .error-item {
            background: #ffebee;
            border: 1px solid #ffcdd2;
            border-left: 4px solid #f44336;
            border-radius: 8px;
            padding: 15px;
            margin-bottom: 15px;
        }
        
        .error-time {
            font-size: 0.9em;
            color: #666;
            margin-bottom: 5px;
            font-family: monospace;
        }
        
        .error-message {
            font-weight: 600;
            color: #d32f2f;
            margin-bottom: 8px;
        }
        
        .error-raw {
            font-family: monospace;
            font-size: 0.85em;
            color: #424242;
            background: #f5f5f5;
            padding: 8px;
            border-radius: 4px;
        }
        
        .no-errors {
            background: #e8f5e8;
            border: 1px solid #4caf50;
            border-radius: 8px;
            padding: 30px;
            text-align: center;
            color: #2e7d32;
        }
        
        .no-errors p {
            font-size: 1.2em;
            font-weight: 500;
        }
        
        .more-errors {
            background: #f0f0f0;
            border-radius: 8px;
            padding: 10px;
            text-align: center;
            margin-top: 10px;
            color: #666;
        }
        
        .footer {
            background: #f8f9fa;
            padding: 20px 30px;
            border-top: 1px solid #e9ecef;
            text-align: center;
            color: #666;
        }
        
        .footer p {
            margin: 5px 0;
        }
        
        @media (max-width: 768px) {
            .stats-grid {
                grid-template-columns: 1fr;
            }
            
            .header h1 {
                font-size: 2em;
            }
            
            .stat-number {
                font-size: 2em;
            }
        }
    )";
} 