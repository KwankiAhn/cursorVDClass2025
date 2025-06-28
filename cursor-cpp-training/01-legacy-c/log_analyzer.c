#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 1024
#define MAX_KEYWORD_LENGTH 64

// 전역 변수들
int total_lines = 0;
int error_count = 0;
int warning_count = 0;
int info_count = 0;

// 로그 레벨 확인 함수
int check_log_level(const char* line) {
    if (strstr(line, "ERROR") != NULL) {
        return 1; // ERROR
    } else if (strstr(line, "WARN") != NULL) {
        return 2; // WARNING
    } else if (strstr(line, "INFO") != NULL) {
        return 3; // INFO
    }
    return 0; // UNKNOWN
}

// 키워드 검색 함수
int search_keyword(const char* line, const char* keyword) {
    return strstr(line, keyword) != NULL;
}

// 통계 출력 함수
void print_statistics() {
    printf("\n=== 로그 분석 결과 ===\n");
    printf("전체 라인 수: %d\n", total_lines);
    printf("ERROR 개수: %d\n", error_count);
    printf("WARNING 개수: %d\n", warning_count);
    printf("INFO 개수: %d\n", info_count);
    printf("분석 완료 시간: %s", ctime(&(time_t){time(NULL)}));
}

// 메인 함수
int main(int argc, char* argv[]) {
    FILE* file;
    char line[MAX_LINE_LENGTH];
    char keyword[MAX_KEYWORD_LENGTH] = "";
    
    // 인자 검사
    if (argc < 2) {
        printf("사용법: %s <로그파일> [검색키워드]\n", argv[0]);
        return 1;
    }
    
    // 키워드가 있으면 저장
    if (argc >= 3) {
        strcpy(keyword, argv[2]);
    }
    
    // 파일 열기
    file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("파일을 열 수 없습니다: %s\n", argv[1]);
        return 1;
    }
    
    printf("로그 파일 분석 시작: %s\n", argv[1]);
    if (strlen(keyword) > 0) {
        printf("검색 키워드: %s\n", keyword);
    }
    
    // 파일 읽기 및 분석
    while (fgets(line, sizeof(line), file)) {
        total_lines++;
        
        // 로그 레벨 분석
        int level = check_log_level(line);
        switch (level) {
            case 1:
                error_count++;
                break;
            case 2:
                warning_count++;
                break;
            case 3:
                info_count++;
                break;
        }
        
        // 키워드 검색 (있는 경우)
        if (strlen(keyword) > 0 && search_keyword(line, keyword)) {
            printf("[키워드 발견] %s", line);
        }
        
        // ERROR 라인 출력
        if (level == 1) {
            printf("[ERROR] %s", line);
        }
    }
    
    // 파일 닫기
    fclose(file);
    
    // 통계 출력
    print_statistics();
    
    return 0;
} 