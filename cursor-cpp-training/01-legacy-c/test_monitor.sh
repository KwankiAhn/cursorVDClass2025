#!/bin/bash

# 테스트용 로그 모니터링 스크립트 (10초 간격)

# 설정
LOG_FILE="../sample-logs/sample.log"
HTML_OUTPUT="report.html"
INTERVAL=10  # 10초 (테스트용)
MAX_RUNS=3   # 최대 3번 실행

# 색상 코드
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 실행 파일 확인
if [ ! -f "./build/log_analyzer" ]; then
    echo -e "${RED}❌ 오류: log_analyzer 실행 파일이 없습니다.${NC}"
    echo "먼저 빌드를 실행하세요:"
    echo "  cd build && make"
    exit 1
fi

# 로그 파일 확인
if [ ! -f "$LOG_FILE" ]; then
    echo -e "${RED}❌ 오류: 로그 파일이 없습니다: $LOG_FILE${NC}"
    exit 1
fi

# 시작 메시지
echo -e "${BLUE}🔍 테스트 모니터링 시작 (${MAX_RUNS}번 실행)${NC}"
echo -e "${BLUE}📁 로그 파일: $LOG_FILE${NC}"
echo -e "${BLUE}🌐 HTML 출력: $HTML_OUTPUT${NC}"
echo -e "${BLUE}⏱️  모니터링 간격: ${INTERVAL}초${NC}"
echo ""

# 카운터 초기화
count=0

# 무한 루프
while [ $count -lt $MAX_RUNS ]; do
    count=$((count + 1))
    
    # 현재 시간
    current_time=$(date '+%Y-%m-%d %H:%M:%S')
    
    echo -e "${GREEN}🔄 분석 #$count - $current_time${NC}"
    
    # 로그 분석 실행
    if ./build/log_analyzer "$LOG_FILE" --html-output "$HTML_OUTPUT" > /dev/null 2>&1; then
        echo -e "   ✅ HTML 리포트 생성 완료: $HTML_OUTPUT"
        
        # 파일 크기 확인
        if [ -f "$HTML_OUTPUT" ]; then
            file_size=$(ls -lh "$HTML_OUTPUT" | awk '{print $5}')
            echo -e "   📊 파일 크기: $file_size"
        fi
    else
        echo -e "   ${RED}❌ 로그 분석 실패${NC}"
    fi
    
    # 마지막 실행이 아니면 대기
    if [ $count -lt $MAX_RUNS ]; then
        echo -e "   💤 ${INTERVAL}초 대기 중..."
        echo ""
        sleep $INTERVAL
    fi
done

echo -e "${BLUE}🎉 테스트 완료!${NC}"
echo -e "${BLUE}📄 최종 리포트: $HTML_OUTPUT${NC}" 