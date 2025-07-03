
## **[팀/프로젝트 명칭 삽입] 코틀린 개발 가이드라인 템플릿 (vX.X)**

**문서 목적:** 이 문서는 `[팀/프로젝트 명칭 삽입]`에서 코틀린(Kotlin)을 사용하여 소프트웨어를 개발할 때 일관성, 가독성, 유지보수성, 성능 및 안정성을 향상시키기 위한 코드 스타일 및 개발 규칙을 정의합니다. 이 가이드라인은 팀 구성원 간의 원활한 협업을 돕고 고품질 소프트웨어 개발을 목표로 합니다.

**최종 수정일:** `[YYYY-MM-DD]`

**버전:** `[X.X]`

**담당자/팀:** `[담당자 또는 팀 이름]`

---

### **1. 기본 원칙 (General Principles)**

* **가독성 (Readability):** 코드는 작성하는 시간보다 읽는 시간이 훨씬 깁니다. 항상 다른 개발자가 쉽게 이해할 수 있도록 작성합니다. `[가독성 관련 팀의 철학이나 구체적인 지향점 명시]`
* **간결성 (Simplicity):** 불필요하게 복잡한 로직이나 구조는 지양합니다. 문제를 해결하는 가장 명확하고 간단한 방법을 우선시합니다. `[KISS, YAGNI 등 적용 여부 명시]`
* **일관성 (Consistency):** 프로젝트 내, 팀 내에서 코드 스타일과 구조를 일관되게 유지합니다. 이 가이드라인을 준수하여 일관성을 확보합니다.
* **안정성 (Robustness):** 오류 발생 가능성을 최소화하고, 발생 시 적절히 처리하여 안정적인 소프트웨어를 만듭니다. Null 안전성, 예외 처리 등을 중요하게 다룹니다.
* **성능 (Performance):** MX 사업부 제품의 특성을 고려하여, 불필요한 리소스 사용(CPU, 메모리, 배터리 등)을 최소화하고 효율적인 코드를 작성합니다. `[특히 중요하게 생각하는 성능 지표나 원칙 명시]`

---

### **2. 코드 스타일 및 포맷팅 (Code Style & Formatting)**

* **기본 규칙:** [Kotlin 공식 코딩 컨벤션](https://kotlinlang.org/docs/coding-conventions.html)을 기본으로 따릅니다.
* **자동 포맷터:** `[사용할 포맷터 명시 (예: ktlint, IntelliJ Formatter)]` 사용을 [권장 / 필수]로 하며, 관련 설정 파일 `[설정 파일 위치 또는 링크]`을 공유합니다.
* **들여쓰기:** `[4칸 공백 / 기타]`
* **줄 길이 제한:** `[100자 / 120자 / 기타]`
* **명명 규칙 (Naming Conventions):**
    * 패키지: `[회사/팀 도메인 역순 + 프로젝트/모듈명 (예: com.samsung.mx.myfeature)]`
    * 클래스/인터페이스: `PascalCase`
    * 함수/변수: `camelCase`
    * 상수 (`const val`, `companion object` 내 `val`): `[UPPER_SNAKE_CASE / camelCase / 혼용 규칙 명시]`
    * 테스트 함수: `` `백틱을 이용한 자연어 방식` `` `[허용 / 권장 / 금지]`
    * `[기타 특수한 네이밍 규칙 추가 (예: ViewModel, UseCase, Repository 등의 접미사 규칙)]`
* **와일드카드 임포트 (`import ...*`)**: `[허용 / 제한적 허용 (예: 표준 라이브러리, 특정 모듈) / 금지]`

---

### **3. 코틀린 언어 기능 활용 (Kotlin Language Features)**

* **Null 안전성 (Null Safety):**
    * Nullable 타입(`?`) 사용을 최소화하되, 필요한 경우 명확히 사용합니다.
    * `!!` (Non-null assertion) 사용 정책: `[엄격히 금지 / 코드 리뷰 시 사유 설명 필수 / 예외적 허용 조건 명시]`
    * 안전 호출(`?.`), 엘비스 연산자(`?:`), `let`/`run`/`apply`/`also` 등을 활용한 Null 처리: `[적극 권장 / 가이드라인 예시 제공]`
* **불변성 (Immutability):**
    * `val` 및 불변 컬렉션 (`List`, `Set`, `Map`) 사용: `[강력히 권장 / 필수 / 예외 조건 명시]`
    * `data class`의 `copy()` 메소드 활용: `[권장]`
* **타입 추론 (Type Inference):**
    * 지역 변수: `[허용 / 권장]`
    * 공개(public/internal) API의 함수 반환 타입 및 프로퍼티 타입: `[명시적 타입 지정 필수 / 권장]`
* **스코프 함수 (`let`, `run`, `with`, `apply`, `also`):**
    * 사용 정책: `[적극 권장 / 남용 금지 / 각 함수 사용 가이드라인 제시]`
    * `[팀 내 선호하는 사용 패턴이나 피해야 할 패턴 명시]`
* **데이터 클래스 (`data class`):**
    * 단순 데이터 전달 목적의 클래스: `[data class 사용 필수 / 권장]`
    * `[equals(), hashCode(), toString(), copy() 자동 생성의 이점 활용 강조]`
* **실드 클래스 (`sealed class` / `sealed interface`):**
    * 제한된 계층 구조 표현 (상태 관리, 결과 처리 등): `[적극 권장 / 사용 패턴 예시 제공]`
* **확장 함수 (Extension Functions):**
    * 사용 정책: `[권장하되, 무분별한 확장 자제 / 유틸리티성 기능, 특정 타입 편의 기능에 사용]`
    * `[확장 함수 정의 위치나 관리 방식 규칙 명시]`
* **코루틴 (Coroutines):**
    * 비동기 처리 기본 방식: `[Coroutine 사용 필수 / 권장]` (아래 '동시성' 섹션 참조)
* **고차 함수 및 람다 (Higher-Order Functions & Lambdas):**
    * 사용 정책: `[적극 활용 권장 / 가독성을 해치지 않는 선에서 사용]`
    * 람다 파라미터 명명: `[의미 있는 이름 사용 권장 / it 사용은 명확한 경우에만 허용]`

---

### **4. 안드로이드 개발 관련 (Android Specifics)**

* **아키텍처 패턴:** `[사용할 아키텍처 패턴 명시 (예: MVVM, MVI)]` 및 관련 라이브러리 `[예: Android Architecture Components (ViewModel, LiveData/StateFlow, Room)]` 사용 규칙.
* **라이프사이클 관리 (Lifecycle Management):**
    * `LifecycleOwner`, `LifecycleObserver` 활용: `[필수 / 권장]`
    * `LifecycleScope`, `repeatOnLifecycle` 사용: `[Coroutine 사용 시 필수 / 권장]`
* **컨텍스트 (Context) 관리:**
    * 메모리 누수 방지를 위한 `ApplicationContext` vs `ActivityContext` 사용 구분: `[명확한 기준 제시]`
    * `[Context 의존성 주입 방식 명시]`
* **UI 스레드 (Main Thread) 작업:**
    * UI 스레드에서는 시간이 오래 걸리는 작업(네트워크, DB 접근, 복잡한 연산) 금지: `[엄격히 금지]`
    * `[Coroutine Dispatchers.Main 사용 규칙 명시]`
* **리소스 관리 (Resource Management):**
    * 문자열(`String`), 크기(`Dimension`), 색상(`Color`) 등 리소스 파일(`xml`) 사용: `[필수]`
    * 리소스 명명 규칙: `[Prefix 사용 등 구체적인 규칙 명시 (예: string_activity_login_title)]`
    * `[다국어, 화면 크기/밀도 등 고려한 리소스 관리 방안 명시]`
* **뷰 바인딩 / 데이터 바인딩 (View Binding / Data Binding):**
    * `findViewById` 사용: `[금지 / 제한적 허용]`
    * `[View Binding / Data Binding 중 선호하는 방식 및 사용 규칙 명시]`
* **성능 최적화 (Performance Optimization):**
    * 메모리 누수 방지: `[LeakCanary 사용 등 탐지 및 해결 방안 명시]`
    * 백그라운드 작업: `[WorkManager 사용 규칙 / 포그라운드 서비스 사용 기준 명시]`
    * 배터리 소모 최적화: `[WakLock 사용 최소화 / 백그라운드 실행 제한 준수 등 관련 규칙 명시]`
    * 앱 시작 시간 최적화: `[Application onCreate() 최적화 등 관련 규칙 명시]`
    * `[기타 MX 제품 특화 성능 관련 주의사항 및 규칙 추가]`

---

### **5. 동시성 (Concurrency)**

* **기본 방식:** 코틀린 코루틴 (`kotlinx.coroutines`) 사용 `[필수 / 권장]`
* **구조화된 동시성 (Structured Concurrency):** `[CoroutineScope`, `viewModelScope`, `lifecycleScope` 등 사용 필수]`
* **Dispatcher 선택:**
    * `Dispatchers.Main`: UI 관련 작업
    * `Dispatchers.IO`: 네트워크, 디스크 I/O 작업
    * `Dispatchers.Default`: CPU 집약적 작업
    * `[Dispatcher 사용 관련 구체적인 가이드라인 명시]`
* **Flow 사용:** Reactive Stream 처리 `[StateFlow, SharedFlow 등 사용 권장 / 사용 패턴 명시]`
* **오류 처리:** 코루틴 내 예외 처리 (`try-catch`, `CoroutineExceptionHandler`) `[가이드라인 및 예시 제공]`
* **취소 (Cancellation):** `[협력적인 취소(cooperative cancellation) 구현 / `ensureActive()`, `yield()` 사용 등 관련 규칙 명시]`

---

### **6. 의존성 관리 (Dependency Management)**

* **의존성 주입 (Dependency Injection):** `[사용할 DI 프레임워크 명시 (예: Hilt, Koin, 수동 DI)]` 및 사용 규칙.
* **라이브러리 버전 관리:** `[버전 카탈로그(Version Catalog) 사용 / buildSrc / 기타 방식 명시]`
* **라이브러리 추가/갱신 정책:** `[코드 리뷰 시 검토 / 보안 취약점 점검 / 라이선스 확인 등 절차 명시]`
* **모듈화 (Modularization):** `[프로젝트 모듈 분리 전략 및 의존성 방향 규칙 명시]`

---

### **7. 테스트 (Testing)**

* **테스트 종류별 작성 가이드:**
    * **단위 테스트 (Unit Tests):** `[JUnit, Mockito/MockK 사용 / 테스트 대상 및 범위 명시]`
    * **통합 테스트 (Integration Tests):** `[테스트 범위 및 환경 구성 방안 명시]`
    * **UI 테스트 (UI Tests):** `[Espresso, Robolectric 사용 / 테스트 전략 명시]`
* **테스트 커버리지 목표:** `[팀/프로젝트별 목표 커버리지 설정 (선택 사항)]`
* **테스트 코드 작성 규칙:** `[테스트 함수 명명 규칙 / Given-When-Then 패턴 등 구조화 방식 권장]`

---

### **8. 문서화 및 주석 (Documentation & Comments)**

* **KDoc:** 공개(public/internal) API (클래스, 함수, 주요 프로퍼티) `[KDoc 작성 필수 / 권장]`
    * `[포함해야 할 내용 명시 (@param, @return, @throws 등)]`
* **구현 주석 (`//`):** 코드만으로 이해하기 어려운 '왜(Why)' 또는 복잡한 로직 설명. `[주석 작성 기준 및 피해야 할 주석 유형 명시]`
* **README 파일:** 모듈 또는 주요 기능 단위 `[README 작성 권장 / 필수 템플릿 제공]`

---

### **9. 오류 처리 및 로깅 (Error Handling & Logging)**

* **예외 처리 전략:** `[Checked Exception vs Unchecked Exception 처리 방식 / Result 클래스 또는 Sealed Class 활용 방안 명시]`
* **로깅 라이브러리:** `[사용할 로깅 라이브러리 명시 (예: Timber)]` 및 로깅 레벨(`Debug`, `Info`, `Warn`, `Error`) 사용 기준.
* **로그 내용:** `[개인 정보 등 민감 정보 로깅 금지 / 문제 해결에 필요한 충분한 정보 포함]`
* **오류 리포팅:** `[사용할 오류 리포팅 도구 (예: Firebase Crashlytics) 및 사용 방식 명시]`

---

### **10. 보안 (Security)**

* **민감 정보 처리:** `[소스 코드 내 하드코딩 금지 / 암호화 저장 / 안전한 API 키 관리 방안 명시]`
* **입력 값 검증 (Input Validation):** `[외부 입력(사용자 입력, 네트워크 응답 등) 검증 필수]`
* **권한 (Permissions):** `[최소 권한 원칙 준수 / 권한 요청 시점 및 사유 설명 가이드라인]`
* **네트워크 보안:** `[HTTPS 사용 필수 / 인증서 검증 / 중간자 공격 방지책]`
* `[기타 MX 플랫폼 또는 서비스 관련 보안 규정 및 가이드라인 명시]`

---

### **11. 코드 리뷰 (Code Review)**

* **코드 리뷰 정책:** `[코드 리뷰 필수 여부 / 리뷰어 수 / 리뷰 요청 방식 명시]`
* **리뷰 중점 사항:** `[가이드라인 준수 여부 / 로직 정확성 / 성능 / 가독성 / 테스트 코드 등 확인 항목 명시]`
* **리뷰 의견 반영:** `[의견 반영 및 재리뷰 절차 명시]`

---

### **12. 가이드라인 관리 (Guideline Management)**

* **업데이트 주기:** `[정기적 (분기/반기) 또는 필요시 업데이트]`
* **개정 제안 및 논의:** `[팀 내 논의 채널 및 방식 명시]`
* **예외 적용:** `[가이드라인을 따르기 어려운 경우 예외 요청 및 승인 절차 명시]`

---

**참고 자료:**

* [Kotlin 공식 문서](https://kotlinlang.org/docs/home.html)
* [Kotlin 코딩 컨벤션](https://kotlinlang.org/docs/coding-conventions.html)
* [Android 개발자 가이드](https://developer.android.com/guide)
* `[팀 내부 공유 문서 또는 위키 링크]`
* `[기타 참고할 만한 외부 자료 링크]`

---

**부록:**

* `[코드 예시 (Good/Bad Practices)]`
* `[자주 묻는 질문 (FAQ)]`
* `[체크리스트]`

---

이 템플릿을 바탕으로 각 팀의 상황과 프로젝트 특성에 맞게 내용을 구체화하고 수정하여 활용하시기 바랍니다.