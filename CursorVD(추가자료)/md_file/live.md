# 실시간 로그 모니터링 시스템 기술 명세서 (강의용 간소화 버전)

## 📋 프로젝트 개요

### 핵심 요구사항

- 로그 파일 실시간 감시
- ERROR 발생 시 알림
- 웹에서 실시간 업데이트

### 프로젝트 목표 (현실적 범위)

기존 LogAnalyzer를 확장하여 **기본적인 실시간 로그 모니터링과 단순한 웹 인터페이스**를 제공하는 교육용 프로젝트

## 🛠 기술 스택 (강의 시간 고려)

### Backend (C++17 기반 - 표준 라이브러리만 사용)

- **C++17**: 기존 LogAnalyzer 코드베이스 활용
- **std::filesystem**: 파일 시스템 감시 및 변경 감지
- **std::thread**: 백그라운드 파일 감시
- **std::chrono**: 시간 기반 폴링
- **nlohmann/json**: JSON 데이터 직렬화 (이미 사용 중)
- **기본 HTTP 서버**: 간단한 소켓 기반 HTTP 서버 (외부 라이브러리 없음)

**선택 이유**:

- 외부 의존성 최소화 (강의 환경 고려)
- 빠른 프로토타이핑 가능
- 교육적 가치 (HTTP 프로토콜 이해)

### Frontend (단순 HTML/JavaScript)

- **기본 HTML5**: 단순한 웹 페이지
- **Vanilla JavaScript**: 외부 프레임워크 없음
- **Server-Sent Events (SSE)**: 실시간 데이터 스트리밍 (WebSocket 대신)
- **기본 CSS**: 간단한 스타일링

**선택 이유**:

- 설정 시간 최소화
- 브라우저 호환성 높음
- 교육 목적에 적합 (기본기 학습)

### 알림 시스템 (간소화)

- **콘솔 출력**: stderr로 ERROR 알림
- **로그 파일**: 별도 alert.log 파일 생성
- **시스템 알림**: 플랫폼별 기본 알림 (선택사항)

## 📐 시스템 아키텍처 (간소화)

### 전체 구조

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Log Files     │───▶│  LogMonitor      │───▶│  Simple Web UI  │
│   (파일 폴링)     │    │  (C++ Backend)   │    │  (HTML/JS)      │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                              │
                              ▼
                       ┌──────────────────┐
                       │  Console Alert   │
                       │  + Log File      │
                       └──────────────────┘
```

### 핵심 컴포넌트 (간소화)

#### 1. LogFileWatcher (C++) - 폴링 방식

```cpp
class LogFileWatcher {
private:
    std::filesystem::path watchPath_;
    std::uintmax_t lastFileSize_;
    std::thread watcherThread_;
    std::atomic<bool> isRunning_;
    std::chrono::milliseconds pollInterval_{1000}; // 1초 폴링

public:
    void startWatching();
    void stopWatching();
    void checkFileChanges();
    std::function<void(const LogEntry&)> onNewLogEntry;
};
```

#### 2. SimpleHttpServer (C++) - 기본 HTTP 서버

```cpp
class SimpleHttpServer {
private:
    int serverSocket_;
    std::thread serverThread_;
    std::atomic<bool> isRunning_;
    std::string webRoot_;

public:
    void start(int port = 8080);
    void stop();
    void handleRequest(int clientSocket);
    void serveStaticFile(int socket, const std::string& path);
    void handleSSE(int socket); // Server-Sent Events
};
```

#### 3. AlertManager (C++) - 간단한 알림

```cpp
class AlertManager {
private:
    std::ofstream alertLogFile_;

public:
    void logError(const LogEntry& entry);
    void printConsoleAlert(const LogEntry& entry);
    void writeAlertToFile(const LogEntry& entry);
};
```

## 🚀 구현 단계별 계획 (현실적 3주 과정)

### Week 1: 파일 감시 시스템

**목표**: 폴링 기반 실시간 로그 파일 변경 감지

**작업 항목** (2-3시간):

- [ ] LogFileWatcher 클래스 구현 (폴링 방식)
- [ ] 파일 크기 변화 감지 로직
- [ ] 기존 LogParser와 통합
- [ ] 기본 단위 테스트

**산출물**:

- `LogFileWatcher.hpp/cpp`
- `test_log_file_watcher.cpp`

### Week 2: 간단한 HTTP 서버 및 웹 UI

**목표**: 기본 HTTP 서버와 HTML 페이지

**작업 항목** (3-4시간):

- [ ] SimpleHttpServer 클래스 구현
- [ ] 정적 HTML/CSS/JS 파일 서빙
- [ ] Server-Sent Events 엔드포인트
- [ ] 기본 웹 페이지 작성

**산출물**:

- `SimpleHttpServer.hpp/cpp`
- `web/index.html`
- `web/style.css`
- `web/app.js`

### Week 3: 통합 및 알림 시스템

**목표**: 전체 시스템 통합 및 기본 알림

**작업 항목** (2-3시간):

- [ ] AlertManager 구현
- [ ] 콘솔 및 파일 기반 알림
- [ ] 전체 시스템 통합
- [ ] 기본 테스트 및 데모

**산출물**:

- `AlertManager.hpp/cpp`
- 통합된 LogMonitor 애플리케이션
- 데모용 샘플 로그

## 🎯 현실적 기술적 도전과제

### 1. 파일 폴링 최적화

**도전과제**: 폴링 방식의 성능 및 리소스 사용
**해결 방안**:

- 적절한 폴링 간격 설정 (1초)
- 파일 크기 변화만 체크
- 증분 읽기 (마지막 읽은 위치 기억)

### 2. 간단한 HTTP 서버 구현

**도전과제**: 표준 라이브러리만으로 HTTP 서버 구현
**해결 방안**:

- 기본 소켓 API 사용
- 간단한 HTTP/1.1 프로토콜 파싱
- 최소한의 헤더 처리

### 3. Server-Sent Events 구현

**도전과제**: WebSocket 없이 실시간 데이터 전송
**해결 방안**:

- SSE 프로토콜 사용 (더 간단함)
- Keep-alive 연결 유지
- JSON 형태로 데이터 스트리밍

## 🧪 테스트 전략 (간소화)

### 단위 테스트 (Catch2)

```cpp
TEST_CASE("LogFileWatcher 폴링 테스트", "[LogFileWatcher]") {
    SECTION("파일 크기 변화 감지") {
        LogFileWatcher watcher("test.log");
        bool changeDetected = false;

        watcher.onNewLogEntry = [&](const LogEntry& entry) {
            changeDetected = true;
        };

        watcher.startWatching();

        // 파일에 새 라인 추가
        std::ofstream file("test.log", std::ios::app);
        file << "2023-12-01 ERROR Test message\n";
        file.close();

        std::this_thread::sleep_for(std::chrono::seconds(2));
        REQUIRE(changeDetected);
    }
}
```

### 통합 테스트

- 파일 변경 → 파싱 → 웹 UI 업데이트 전체 플로우
- HTTP 서버 기본 기능 테스트
- SSE 연결 및 데이터 스트리밍 테스트

## ⚡ 성능 고려사항 (현실적)

### 1. 폴링 방식 최적화

**목표**: CPU 사용률 < 5%
**전략**:

- 1초 폴링 간격 (리소스 vs 반응성 균형)
- 파일 크기만 체크 (메타데이터 읽기)
- 변화가 없으면 즉시 sleep

### 2. 메모리 사용량

**목표**: 추가 메모리 사용량 < 10MB
**전략**:

- 증분 파일 읽기 (전체 파일 로드 금지)
- 최근 N개 로그만 메모리 유지
- 정적 HTML 파일 캐싱

## 📦 간단한 배포

### 빌드 시스템 (CMake 확장)

```cmake
# 기존 CMakeLists.txt에 추가
option(BUILD_LIVE_MONITOR "Build live monitoring features" ON)

if(BUILD_LIVE_MONITOR)
    add_executable(LogMonitor
        main.cpp
        LogFileWatcher.cpp
        SimpleHttpServer.cpp
        AlertManager.cpp
        # 기존 소스들...
    )

    # 웹 파일들을 빌드 디렉토리로 복사
    file(COPY web/ DESTINATION ${CMAKE_BINARY_DIR}/web)
endif()
```

### 실행 방법

```bash
# 빌드
cd build && make

# 실행 (포트 8080에서 웹 서버 시작)
./LogMonitor ../sample-logs/sample.log

# 웹 브라우저에서 접속
open http://localhost:8080
```

## 📱 웹 UI 구성 (간단한 HTML)

### index.html (기본 구조)

```html
<!DOCTYPE html>
<html>
  <head>
    <title>Log Monitor</title>
    <link rel="stylesheet" href="style.css" />
  </head>
  <body>
    <h1>실시간 로그 모니터</h1>
    <div id="status">연결 중...</div>
    <div id="errorCount">ERROR: 0개</div>
    <div id="logContainer">
      <!-- 실시간 로그가 여기에 표시 -->
    </div>
    <script src="app.js"></script>
  </body>
</html>
```

### app.js (Server-Sent Events)

```javascript
const eventSource = new EventSource("/events");
let errorCount = 0;

eventSource.onmessage = function (event) {
  const logEntry = JSON.parse(event.data);
  addLogToUI(logEntry);

  if (logEntry.level === "ERROR") {
    errorCount++;
    document.getElementById(
      "errorCount"
    ).textContent = `ERROR: ${errorCount}개`;
  }
};

function addLogToUI(logEntry) {
  const logDiv = document.createElement("div");
  logDiv.className = `log-entry ${logEntry.level.toLowerCase()}`;
  logDiv.textContent = `${logEntry.timestamp} [${logEntry.level}] ${logEntry.message}`;

  document.getElementById("logContainer").prepend(logDiv);
}
```

## 🎓 교육적 가치

### 학습 목표

1. **파일 I/O와 폴링**: 실시간 시스템의 기본 개념
2. **소켓 프로그래밍**: HTTP 서버 직접 구현
3. **웹 기술 통합**: C++와 웹의 연계
4. **비동기 처리**: 멀티스레딩 활용

### 확장 가능성 (시간이 있다면)

- 더 정교한 파일 감시 (inotify/kqueue)
- WebSocket 업그레이드
- 더 풍부한 웹 UI
- 설정 파일 기반 알림 규칙

---

_이 간소화된 버전은 강의 시간 제약을 고려하여 3주 내에 완성 가능한 현실적인 계획입니다._
