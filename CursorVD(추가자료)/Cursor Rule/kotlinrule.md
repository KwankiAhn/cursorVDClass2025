## **Cursor AI용 코틀린 코드 생성 및 수정 가이드라인**

이 가이드라인은 Cursor AI가 코틀린 코드를 생성하거나 수정할 때 일관성, 가독성, 안정성 및 최신성을 유지하도록 돕기 위해 작성되었습니다. AI는 다음 규칙들을 최대한 준수하여 코드를 작성해야 합니다.

**1. 최신 안정 버전의 코틀린 기능 우선 사용 (Use Modern Kotlin Features)**

* 항상 안정화된 최신 버전의 코틀린에서 제공하는 문법과 기능을 활용합니다.
* 예: `when` 표현식, 데이터 클래스(`data class`), 실드 클래스(`sealed class`), 확장 함수(`extension function`), 코루틴(`Coroutine`) 등을 적극적으로 사용합니다.
* Java와의 상호운용성을 위한 코드가 아니라면, 코틀린 고유의 기능을 우선적으로 고려합니다.

**2. Null 안전성 적극 활용 및 강제 언래핑 최소화 (Leverage Null Safety, Minimize Force Unwrapping)**

* Nullable 타입(`?`)을 명확히 사용하고, non-null 타입으로 안전하게 변환하는 방법을 우선시합니다.
* `?.` (안전 호출), `?:` (엘비스 연산자), `let`, `run`, `apply`, `also` 등의 스코프 함수를 활용하여 null 처리를 간결하고 안전하게 수행합니다.
* `!!` (non-null assertion) 연산자는 사용을 **최대한 지양**합니다. 사용해야만 하는 명확한 이유가 있고 해당 변수가 절대 null이 될 수 없음을 보장할 수 있는 경우에만 제한적으로 사용하며, 사용 시 주석으로 이유를 설명합니다.

**3. 불변성(Immutability) 적극 활용 (Embrace Immutability)**

* 변경 가능한 변수(`var`) 대신 변경 불가능한 변수(`val`) 사용을 **우선시**합니다.
* 변경 불가능한 컬렉션(`List`, `Set`, `Map`)을 기본으로 사용하고, 명시적으로 변경이 필요한 경우에만 변경 가능한 컬렉션(`MutableList`, `MutableSet`, `MutableMap`)을 사용합니다.

**4. 코틀린 표준 라이브러리 함수 적극 활용 (Utilize Standard Library Functions)**

* 반복문(`for`, `while`) 대신 `forEach`, `map`, `filter`, `flatMap`, `fold` 등과 같은 컬렉션 처리 함수를 적극적으로 사용합니다.
* 스코프 함수(`let`, `run`, `with`, `apply`, `also`)를 적절한 맥락에서 사용하여 코드의 가독성과 간결성을 높입니다. 각 함수의 미묘한 차이(수신 객체, 반환 값)를 이해하고 용도에 맞게 사용합니다.

**5. 코틀린 공식 네이밍 컨벤션 준수 (Adhere to Kotlin Naming Conventions)**

* **클래스 및 인터페이스:** PascalCase (`MyClass`, `MyInterface`)
* **함수 및 변수:** camelCase (`myFunction`, `myVariable`)
* **상수(compile-time constants):** `const val` 사용 시 UPPER_SNAKE_CASE (`MAX_COUNT`)
* **일반적인 `val` 상수:** camelCase (`defaultTimeout`) - 상황에 따라 UPPER_SNAKE_CASE 사용 가능 (예: `companion object` 내)
* **테스트 함수:** 백틱(`` ` ``)을 사용하여 자연스러운 문장 형태로 작성 가능 (`fun `test addition with negative numbers`()`)

**6. 일관된 코드 포맷팅 유지 (Maintain Consistent Formatting)**

* 들여쓰기는 **4칸 공백**을 사용합니다. (탭 사용 금지)
* 줄 길이는 일반적으로 100~120자 이내로 제한합니다.
* 연산자 주위, 쉼표 뒤, 콜론 뒤 등에는 공백을 추가하여 가독성을 높입니다.
* [Kotlin 공식 코딩 컨벤션](https://kotlinlang.org/docs/coding-conventions.html) 또는 프로젝트/팀에서 사용하는 `ktlint`, IntelliJ Formatter 등의 규칙을 따릅니다.

**7. 명확하고 간결한 주석 및 KDoc 작성 (Write Clear Comments and KDoc)**

* **구현 주석 (`//`):** 코드만으로 이해하기 어려운 로직의 '왜(why)'를 설명합니다. 코드가 '무엇(what)'을 하는지 단순히 반복 설명하는 주석은 지양합니다.
* **KDoc (`/** */`):** 공개(public) API (클래스, 함수, 프로퍼티 등)에는 KDoc 문서를 작성하여 사용법, 파라미터, 반환 값, 발생 가능한 예외 등을 명확히 설명합니다.
* 불필요하거나 오래된 주석은 제거합니다.

**8. 오류 처리 명확화 (Clarify Error Handling)**

* 예외를 발생시킬 가능성이 있는 경우, `try-catch` 블록을 사용하여 적절히 처리합니다.
* 무분별하게 `catch (e: Exception)` 또는 `catch (e: Throwable)`을 사용하는 것을 지양하고, 예측 가능한 특정 예외 타입을 명시하여 처리합니다.
* 함수의 결과로 성공 또는 실패를 나타내야 할 경우, 코틀린 `Result` 클래스나 `sealed class`를 활용하는 것을 고려합니다.

**9. 타입 추론 활용 및 명시적 타입 지정 균형 (Balance Type Inference and Explicit Types)**

* 지역 변수 등 명확한 경우에는 타입 추론을 활용하여 코드를 간결하게 만듭니다.
* 함수의 반환 타입이나 공개 API의 프로퍼티 타입은 **명시적으로 지정**하여 가독성과 코드 이해도를 높입니다.

**10. 간결하고 읽기 쉬운 코드 작성 (Write Concise and Readable Code)**

* 지나치게 중첩된 블록이나 복잡한 표현식은 피합니다. 필요한 경우 함수로 분리하거나 변수를 사용하여 가독성을 개선합니다.
* 함수는 가능한 한 작고 단일 책임 원칙(Single Responsibility Principle)을 따르도록 작성합니다.

---

