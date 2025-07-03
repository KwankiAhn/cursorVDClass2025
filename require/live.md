# 실시간 로그 모니터링 시스템 기술 명세서 (교육용 초간단 버전)

## 📋 핵심 요구사항 (최소화)
- 로그 파일 주기적 감시 (1분마다)
- ERROR 발생 시 HTML 리포트 생성
- 웹브라우저에서 결과 확인 (수동 새로고침)

## 🏗️ 시스템 아키텍처 (초간단)

### 전체 구조
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   로그 파일      │◄──►│   C++ 프로그램   │──►│   HTML 파일     │
│   (sample.log)  │    │   (주기적 실행)  │    │ (정적 파일)     │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                                        │
                                                        ▼
                                                ┌─────────────────┐
                                                │   웹 브라우저   │
                                                │   (수동 새로고침)│
                                                └─────────────────┘
```

## 🛠️ 기술 스택 선택 (초간단)

### 백엔드 (로그 분석)
- **C++ 확장**: 기존 로그 분석기에 HTML 생성 기능 추가
- **cron 또는 스케줄러**: 1분마다 프로그램 실행

### 프론트엔드 (결과 확인)
- **순수 HTML/CSS**: 정적 HTML 파일 생성
- **수동 새로고침**: F5 키로 최신 상태 확인

### 데이터 저장
- **HTML 파일**: 분석 결과를 HTML 파일로 출력
- **없음**: 별도 데이터베이스나 서버 불필요

## 📈 구현 단계별 계획 (초간단)

### 1단계: C++ HTML 생성 기능 추가 (1일)
**목표**: 기존 로그 분석기에 HTML 출력 기능 추가

**작업 내용**:
- `HTMLReporter` 클래스 구현
  - 로그 통계를 HTML 형식으로 출력
  - ERROR 발생 시 빨간색 하이라이트
  - 마지막 업데이트 시간 표시

**구현 예시**:
```cpp
class HTMLReporter {
private:
    std::string outputPath_;
    
public:
    HTMLReporter(const std::string& outputPath) : outputPath_(outputPath) {}
    
    void generateReport(const LogStats& stats) {
        std::ofstream file(outputPath_);
        file << generateHTMLContent(stats);
        file.close();
    }
    
private:
    std::string generateHTMLContent(const LogStats& stats) {
        return R"(<!DOCTYPE html>
<html>
<head>
    <title>로그 모니터링 리포트</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .error { color: red; font-weight: bold; }
        .stats { background: #f0f0f0; padding: 10px; margin: 10px 0; }
        .timestamp { color: #666; font-size: 0.9em; }
    </style>
</head>
<body>
    <h1>로그 모니터링 리포트</h1>
    <div class="timestamp">마지막 업데이트: )" + getCurrentTime() + R"(</div>
    
    <div class="stats">
        <h2>로그 통계</h2>
        <p>총 로그 수: )" + std::to_string(stats.getTotalCount()) + R"(</p>
        <p>INFO: )" + std::to_string(stats.getInfoCount()) + R"(</p>
        <p>WARN: )" + std::to_string(stats.getWarnCount()) + R"(</p>
        <p class="error">ERROR: )" + std::to_string(stats.getErrorCount()) + R"(</p>
    </div>
    
    <div class="recent-errors">
        <h2>최근 ERROR 로그</h2>
        )" + generateErrorList(stats.getRecentErrors()) + R"(
    </div>
</body>
</html>)";
    }
};
```

### 2단계: 자동 실행 스크립트 작성 (0.5일)
**목표**: 주기적으로 로그 분석 및 HTML 생성

**작업 내용**:
- 간단한 쉘 스크립트 작성
- 1분마다 C++ 프로그램 실행
- HTML 파일 생성 확인

**구현 예시**:
```bash
#!/bin/bash
# monitor_loop.sh

while true; do
    echo "로그 분석 중... $(date)"
    ./build/log_analyzer sample-logs/sample.log --html-output=report.html
    echo "HTML 리포트 생성 완료"
    sleep 60  # 1분 대기
done
```

### 3단계: 간단한 HTML 스타일링 (0.5일)
**목표**: 보기 좋은 HTML 리포트 생성

**작업 내용**:
- 기본적인 CSS 스타일 적용
- ERROR 발생 시 빨간색 강조
- 반응형 디자인 (선택사항)

**HTML 템플릿 예시**:
```html
<!DOCTYPE html>
<html>
<head>
    <title>로그 모니터링 리포트</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            margin: 0;
            padding: 20px;
            background-color: #f5f5f5;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
            background: white;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            text-align: center;
            margin-bottom: 30px;
        }
        .timestamp {
            text-align: center;
            color: #666;
            font-size: 0.9em;
            margin-bottom: 30px;
        }
        .stats {
            background: #f8f9fa;
            padding: 20px;
            border-radius: 5px;
            margin: 20px 0;
        }
        .stat-item {
            display: flex;
            justify-content: space-between;
            margin: 10px 0;
            padding: 8px;
            border-radius: 3px;
        }
        .stat-item.error {
            background: #ffebee;
            color: #c62828;
            font-weight: bold;
        }
        .stat-item.warn {
            background: #fff3e0;
            color: #ef6c00;
        }
        .stat-item.info {
            background: #e3f2fd;
            color: #1976d2;
        }
        .error-list {
            margin-top: 20px;
        }
        .error-item {
            background: #ffebee;
            border-left: 4px solid #f44336;
            padding: 10px;
            margin: 10px 0;
            border-radius: 3px;
        }
        .error-time {
            font-size: 0.9em;
            color: #666;
        }
        .refresh-notice {
            text-align: center;
            background: #e8f5e8;
            padding: 10px;
            border-radius: 5px;
            margin: 20px 0;
            color: #2e7d32;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🔍 로그 모니터링 리포트</h1>
        <div class="timestamp">마지막 업데이트: {{TIMESTAMP}}</div>
        
        <div class="refresh-notice">
            💡 최신 정보를 보려면 F5 키를 눌러 새로고침하세요
        </div>
        
        <div class="stats">
            <h2>📊 로그 통계</h2>
            <div class="stat-item info">
                <span>INFO</span>
                <span>{{INFO_COUNT}}</span>
            </div>
            <div class="stat-item warn">
                <span>WARN</span>
                <span>{{WARN_COUNT}}</span>
            </div>
            <div class="stat-item error">
                <span>ERROR</span>
                <span>{{ERROR_COUNT}}</span>
            </div>
        </div>
        
        <div class="error-list">
            <h2>🚨 최근 ERROR 로그</h2>
            {{ERROR_LIST}}
        </div>
    </div>
</body>
</html>
```

## 🧪 테스트 방법 (초간단)

### 1. 기본 테스트
```bash
# 1. 로그 파일에 ERROR 추가
echo "$(date '+%Y-%m-%d %H:%M:%S') ERROR: 테스트 에러 메시지" >> sample-logs/sample.log

# 2. 수동으로 프로그램 실행
./build/log_analyzer sample-logs/sample.log --html-output=report.html

# 3. 브라우저에서 HTML 파일 열기
open report.html  # macOS
# firefox report.html  # Linux
```

### 2. 자동 모니터링 테스트
```bash
# 1. 자동 모니터링 스크립트 실행
./monitor_loop.sh &

# 2. 로그 파일에 ERROR 추가
echo "$(date '+%Y-%m-%d %H:%M:%S') ERROR: 또 다른 테스트 에러" >> sample-logs/sample.log

# 3. 1분 후 브라우저에서 새로고침 확인
```

## ⚡ 성능 고려사항 (초간단)

### 1. 기본 성능 목표
- **분석 주기**: 1분마다 실행
- **HTML 생성**: 1초 이내
- **파일 크기**: HTML 파일 100KB 이하

### 2. 간단한 최적화
- 최근 10개 ERROR만 표시
- 로그 파일 크기가 큰 경우 마지막 1000줄만 분석
- HTML 템플릿 재사용

## 🚀 실행 방법 (초간단)

### 1. 수동 실행
```bash
# 1. C++ 프로그램 빌드
cd cursor-cpp-training/01-legacy-c
mkdir build && cd build
cmake ..
make

# 2. HTML 리포트 생성
./log_analyzer ../sample-logs/sample.log --html-output=../report.html

# 3. 브라우저에서 리포트 확인
open ../report.html
```

### 2. 자동 모니터링
```bash
# 1. 모니터링 스크립트 실행
./monitor_loop.sh &

# 2. 브라우저에서 리포트 열기
open report.html

# 3. 필요할 때 F5로 새로고침
```

### 3. 종료 방법
```bash
# 모니터링 스크립트 종료
pkill -f monitor_loop.sh
```

## 📊 성공 지표 (초간단)

### 1. 기능 확인
- ✅ 로그 파일 분석 후 HTML 생성
- ✅ ERROR 발생 시 빨간색으로 강조 표시
- ✅ 브라우저에서 리포트 정상 출력

### 2. 사용성 확인
- ✅ HTML 파일 크기 100KB 이하
- ✅ 프로그램 실행 시간 3초 이내
- ✅ 브라우저에서 읽기 쉬운 형태로 표시

## 🎯 마일스톤 (초간단)

| 시간 | 목표 | 완료 기준 |
|------|------|-----------|
| 1시간 | HTMLReporter 클래스 구현 | HTML 파일 생성 성공 |
| 30분 | 자동 실행 스크립트 작성 | 1분마다 자동 실행 |
| 30분 | HTML 스타일링 개선 | 보기 좋은 리포트 완성 |
| 30분 | 전체 테스트 | 수동/자동 모두 정상 동작 |

**총 소요 시간: 2.5시간**

## 🎓 교육 효과

### 1. 학습 목표
- **파일 출력**: HTML 파일 생성 방법 학습
- **템플릿 시스템**: 간단한 문자열 치환 방식 이해
- **자동화**: 스크립트를 통한 반복 작업 자동화

### 2. 확장 가능성
- 더 예쁜 CSS 스타일 적용
- 차트나 그래프 추가 (Chart.js 등)
- 이메일 알림 기능 추가
- 웹 서버 추가하여 실시간 업데이트 구현

이 초간단 버전은 **2-3시간 내에 완성 가능**하며, 복잡한 웹 기술 없이도 실용적인 로그 모니터링 시스템을 구축할 수 있습니다!