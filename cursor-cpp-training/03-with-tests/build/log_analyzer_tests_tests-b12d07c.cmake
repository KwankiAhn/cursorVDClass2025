add_test( [==[테스트 환경 확인]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[테스트 환경 확인]==]  )
set_tests_properties( [==[테스트 환경 확인]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
add_test( [==[LogFileReader 기본 기능 테스트]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[LogFileReader 기본 기능 테스트]==]  )
set_tests_properties( [==[LogFileReader 기본 기능 테스트]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
add_test( [==[LogFileReader 순차 읽기 테스트]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[LogFileReader 순차 읽기 테스트]==]  )
set_tests_properties( [==[LogFileReader 순차 읽기 테스트]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
add_test( [==[LogFileReader 빈 파일 처리]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[LogFileReader 빈 파일 처리]==]  )
set_tests_properties( [==[LogFileReader 빈 파일 처리]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
add_test( [==[LogFileReader 대용량 파일 시뮬레이션]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[LogFileReader 대용량 파일 시뮬레이션]==]  )
set_tests_properties( [==[LogFileReader 대용량 파일 시뮬레이션]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
add_test( [==[LogParser 기본 기능 테스트]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[LogParser 기본 기능 테스트]==]  )
set_tests_properties( [==[LogParser 기본 기능 테스트]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
add_test( [==[LogParser 여러 라인 파싱 테스트]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[LogParser 여러 라인 파싱 테스트]==]  )
set_tests_properties( [==[LogParser 여러 라인 파싱 테스트]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
add_test( [==[LogParser 키워드 필터링 테스트]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[LogParser 키워드 필터링 테스트]==]  )
set_tests_properties( [==[LogParser 키워드 필터링 테스트]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
add_test( [==[LogParser 레벨 필터링 테스트]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[LogParser 레벨 필터링 테스트]==]  )
set_tests_properties( [==[LogParser 레벨 필터링 테스트]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
add_test( [==[LogParser 문자열 변환 테스트]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[LogParser 문자열 변환 테스트]==]  )
set_tests_properties( [==[LogParser 문자열 변환 테스트]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
add_test( [==[LogStats 통계 계산 테스트]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[LogStats 통계 계산 테스트]==]  )
set_tests_properties( [==[LogStats 통계 계산 테스트]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
add_test( [==[LogStats JSON 출력 테스트]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[LogStats JSON 출력 테스트]==]  )
set_tests_properties( [==[LogStats JSON 출력 테스트]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
add_test( [==[LogStats 통계 출력 테스트]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[LogStats 통계 출력 테스트]==]  )
set_tests_properties( [==[LogStats 통계 출력 테스트]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
add_test( [==[LogStats 레벨별 엔트리 출력 테스트]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[LogStats 레벨별 엔트리 출력 테스트]==]  )
set_tests_properties( [==[LogStats 레벨별 엔트리 출력 테스트]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
add_test( [==[LogStats 키워드 매칭 출력 테스트]==] /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build/log_analyzer_tests [==[LogStats 키워드 매칭 출력 테스트]==]  )
set_tests_properties( [==[LogStats 키워드 매칭 출력 테스트]==] PROPERTIES WORKING_DIRECTORY /Users/browoo/Documents/cursorVD/cursor-cpp-training/03-with-tests/build)
set( log_analyzer_tests_TESTS [==[테스트 환경 확인]==] [==[LogFileReader 기본 기능 테스트]==] [==[LogFileReader 순차 읽기 테스트]==] [==[LogFileReader 빈 파일 처리]==] [==[LogFileReader 대용량 파일 시뮬레이션]==] [==[LogParser 기본 기능 테스트]==] [==[LogParser 여러 라인 파싱 테스트]==] [==[LogParser 키워드 필터링 테스트]==] [==[LogParser 레벨 필터링 테스트]==] [==[LogParser 문자열 변환 테스트]==] [==[LogStats 통계 계산 테스트]==] [==[LogStats JSON 출력 테스트]==] [==[LogStats 통계 출력 테스트]==] [==[LogStats 레벨별 엔트리 출력 테스트]==] [==[LogStats 키워드 매칭 출력 테스트]==])
