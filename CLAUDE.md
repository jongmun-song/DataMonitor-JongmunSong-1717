# CLAUDE.md

이 파일은 이 저장소에서 작업할 때 Claude Code(claude.ai/code)에게 제공되는 가이드입니다.

## 저장소 목적

이 저장소는 `../ref/requirements.pdf`에 기술된 "반도체 시료 생산주문관리 시스템" 과제의 **"데이터 모니터링 Tool" PoC**입니다. 요구사항 원문(Chapter 3, [미션1])의 정의는 다음과 같습니다.

> 데이터 모니터링 Tool — 현재 저장된 데이터 상태를 콘솔에서 실시간 조회할 수 있는 관리자 도구

`SSemiProductManager/` 아래의 5개 형제 PoC 저장소 중 하나입니다.
- `ConsoleMVC` — Model/Controller/View 구조 및 역할 분리 (재사용 가능한 콘솔 MVC 골격)
- `DataPersistence` — 저장소/CRUD 처리 (파일, JSON, 또는 DB)
- `DataMonitor` (본 저장소) — 저장된 데이터를 실시간으로 조회하는 콘솔 관리자 도구
- `DummyDataGenerator` — 영속성 계층에 더미 데이터를 채워 넣는 도구
- `SampleOrderSystem` — 실제 비즈니스 애플리케이션(주문/생산/재고 흐름)으로, 위 PoC 모듈들을 조합해 구축될 것으로 예상됨

**이 저장소는 PoC(Proof of Concept)입니다.** 목표는 추상적으로 재사용 가능한 범용 모듈을 먼저 설계하는 것이 아니라, `../ref/requirements.pdf`에 실제로 적힌 데이터와 화면(시료/주문, 상태값 RESERVED/REJECTED/PRODUCING/CONFIRMED/RELEASE, 재고 수량 등)을 기준으로 "데이터 모니터링 Tool"이 동작하는 것을 개발 단계별로 검증하는 것입니다. 구체적으로:

- Chapter 2의 "모니터링" 메뉴(상태별 주문 수, 시료별 재고 현황 — 요구사항 pp.18-19)에 나오는 시료(Sample)/주문(Order) 데이터와 상태값을 그대로 이 저장소의 구현 대상으로 사용하세요. 참고용 예시가 아니라 실제로 구현해야 할 화면입니다.
- 각 개발 단계가 끝날 때마다, 지금 무엇이 동작하는지(어떤 메뉴/조회가 콘솔에서 실제로 확인되는지)를 아래 "개발 단계" 절의 체크리스트에 비추어 확인할 수 있어야 합니다. 즉, 코드는 항상 "지금 이 단계까지는 콘솔에서 실행해서 눈으로 확인 가능한" 상태를 유지해야 합니다.
- 하드코딩된 절대 경로, 이 저장소 밖에서는 의미가 없는 임시 디버그 코드는 피하되, `SampleOrderSystem` 등 다른 프로젝트가 이 코드를 참고하거나 가져다 쓸 가능성을 고려해 파일/네임스페이스 구성을 지나치게 얽어놓지 않는 정도면 충분합니다. 재사용성을 위해 도메인 로직을 인위적으로 추상화하는 것보다, 요구사항 문서의 시나리오를 정확히 구현하는 것을 우선하세요.
- 가능하다면 `ConsoleMVC`가 정의하는 콘솔 UI 패턴(목록 조회, 페이지네이션, 상태 배지 등)을 참고해 화면을 구성하되, 필수는 아닙니다.

## 데이터 모델 & 모니터링 대상 데이터

데이터 모델과 더미 데이터는 이미 이 저장소에 준비되어 있습니다 — 새로 정의하지 않고 그대로 사용합니다.

- **`dataModel/`** — `Sample.h`, `Order.h`, `ProductionQueueEntry.h`. `DataPersistence::Model` 네임스페이스로 되어 있으며, `NLOHMANN_DEFINE_TYPE_INTRUSIVE`/`NLOHMANN_JSON_SERIALIZE_ENUM`로 JSON과 상호 변환됩니다.
- **`storedData/`** — `samples.json`, `orders.json`, `production_queue.json`. 각각 위 세 모델의 JSON 객체 배열이며, `../DataPersistence`가 실제로 읽고 쓰는(또는 앞으로 읽고 쓸) 형식과 동일한 스키마입니다. 이 저장소가 조회하는 "저장된 데이터"는 추상적인 임의의 데이터가 아니라 이 파일들 그 자체입니다 — `../DataPersistence`를 실행해 만든 실제 `samples.json`으로 교체하면 곧바로 실제 데이터를 조회할 수 있습니다.
- `dataModel/*.h`는 `#include <nlohmann/json.hpp>`에 의존하므로, `../DataPersistence/external/nlohmann/json.hpp`와 동일한 방식으로 이 저장소에도 라이브러리를 vendored로 연동해야 합니다(Phase 0, `docs/design/phase0-foundation.md` 참고).
- 자세한 필드/파일 대응 관계는 [`docs/feature/data-source.md`](docs/feature/data-source.md)를 참고하세요.

## 개발 단계(Phase)로 진행 상황을 확인합니다

이 PoC는 [`docs/PLAN.md`](docs/PLAN.md)에 정의된 phase 순서대로 진행하며, **각 phase가 끝날 때마다 빌드·실행 가능한 콘솔 애플리케이션**을 산출물로 남깁니다. 다음 phase는 이전 phase가 남긴 실행 가능한 상태 위에 기능을 덧붙이는 방식으로 진행하므로, 언제든 "지금 몇 번째 phase까지 실행 가능한 상태인지"로 진행 상황을 확인할 수 있습니다.

- Phase 0: 프로젝트 뼈대 + 데이터 모델/데이터소스 연동 — [`docs/design/phase0-foundation.md`](docs/design/phase0-foundation.md)
- Phase 1: 시료 관리 조회(p.13) — [`docs/design/phase1-sample-view.md`](docs/design/phase1-sample-view.md)
- Phase 2: 주문 리스트 조회(p.17) — [`docs/design/phase2-order-list-view.md`](docs/design/phase2-order-list-view.md)
- Phase 3: 모니터링 요약(p.19) — [`docs/design/phase3-monitoring-summary.md`](docs/design/phase3-monitoring-summary.md)
- Phase 4: 생산라인 조회(p.21) — [`docs/design/phase4-production-line-view.md`](docs/design/phase4-production-line-view.md)
- Phase 5: 출고 가능 조회(p.23) — [`docs/design/phase5-release-candidates-view.md`](docs/design/phase5-release-candidates-view.md)
- Phase 6: 실시간 갱신 통합 — [`docs/design/phase6-live-refresh.md`](docs/design/phase6-live-refresh.md)

각 phase 문서에는 목표, 데이터소스/콘솔 UI에 추가할 내용, 메뉴 구성, 그리고 **실행으로 확인해야 하는 완료 기준**이 함께 정의되어 있습니다. 어떤 phase든 완료 기준(빌드 성공 + 실제 실행 확인)을 만족하지 못하면 다음 phase로 넘어가지 않습니다.

## 실시간 조회 요구사항

요구사항 문서상 이 도구의 본질은 "정적인 1회성 출력"이 아니라 **실시간(계속 갱신되는) 조회**입니다. 다음을 반드시 지원하도록 설계하세요.

- 콘솔에서 반복적으로 최신 데이터 상태를 갱신해서 보여주는 루프(예: 일정 주기 자동 갱신, 또는 사용자의 갱신 명령 입력에 반응하는 수동 갱신) 중 최소 하나 이상.
- 조회 시점의 타임스탬프 표기(요구사항 예시 UI에서도 `모니터링 2026-04-16 09:32:15`처럼 조회 시각을 함께 표기, p.19).
- 저장된 데이터가 외부에서 변경되더라도(다른 프로세스/모듈이 CRUD를 수행하더라도) 재조회 시 최신 상태를 반영해야 하며, 도구 내부에 데이터를 캐싱해 stale 상태로 남겨두지 않아야 합니다.

## 현재 상태

`dataModel/`(데이터 모델)과 `storedData/`(더미 JSON 데이터)는 이미 준비되어 있지만, 아직 `DataMonitor.vcxproj`에 등록되어 있지 않고 리포지토리/콘솔 UI 소스 코드는 없는 상태입니다(`DataMonitor.vcxproj` 끝의 비어 있는 `<ItemGroup>`, `.vcxproj.filters`의 비어 있는 필터 그룹 참고). `nlohmann/json.hpp`도 아직 이 저장소에 vendored되어 있지 않습니다 — Phase 0(`docs/design/phase0-foundation.md`)에서 연동합니다. 새 소스 파일을 추가할 때는 `DataMonitor.vcxproj`(`<ClCompile>`/`<ClInclude>`가 포함된 `<ItemGroup>`)와 `DataMonitor.vcxproj.filters`(“소스 파일” / “헤더 파일”로 분류) 양쪽 모두에 등록하세요.

## 빌드

Visual Studio C++ 프로젝트(`DataMonitor.vcxproj` / `DataMonitor.slnx`)이며, C++20(`stdcpp20`)을 대상으로 `x86`/`x64`, Debug/Release 구성의 Win32 `Application`(콘솔 서브시스템)으로 빌드됩니다.

- `DataMonitor.slnx`를 Visual Studio(v145 툴셋)에서 열어 빌드/실행하거나,
- 다음과 같이 명령줄에서 MSBuild로 빌드할 수 있습니다:
  ```
  msbuild DataMonitor.vcxproj /p:Configuration=Debug /p:Platform=x64
  ```
- 빌드된 실행 파일은 `x64\Debug\DataMonitor.exe`에서 실행하세요(경로는 구성/플랫폼에 따라 다름).

테스트는 gtest(`packages.config`의 `Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn` 1.8.1.8, NuGet 네이티브 패키지)를 사용합니다. 아직 린트 설정은 없습니다.

## 아키텍처 가이드 (requirements.pdf p.13/17/19/21/23, Chapter 3 — PoC 범위 기준)

이 도구가 실제로 구현해야 하는 화면(요구사항 예시 UI 기준, 화면 레이아웃 자체는 자유)은 크게 두 축입니다.

- **주문량(상태별 건수) 확인** — 상태(RESERVED/CONFIRMED/PRODUCING/RELEASE)별 주문 건수 요약. REJECTED는 무효 주문이므로 집계에서 제외한다.
- **재고량 확인** — 시료별 현재 재고 수량과, 그 값에 대한 임계치 기반 상태(여유/부족/고갈)·잔여율 표기.

권장 계층 분리(위 "개발 단계"와 대응):

- **데이터 계층** — `dataModel/`의 Sample/Order/ProductionQueueEntry 구조체와, `storedData/`의 JSON 파일을 읽는 데이터소스(`SampleDataSource` 등). 조회 함수(`reload()`/`all()`)를 통해서만 데이터에 접근한다(직접 배열/전역변수를 여기저기서 참조하지 않는다).
- **집계/요약 로직** — 상태별 카운트, 재고 임계치 판정, 갱신 시각 관리.
- **콘솔 출력(View)** — 요약 정보를 표/배지 형태로 렌더링하고, 갱신 루프와 사용자 입력(메뉴 선택, 갱신, 뒤로가기/종료)을 처리.

이 계층 분리는 "도메인을 몰라도 되게" 만들기 위함이 아니라, `storedData/`를 `DataPersistence`가 실제로 만든 JSON 파일로 교체하거나 `DummyDataGenerator`/`SampleOrderSystem`과 연동할 때도 데이터 계층만 교체하면 되도록 하기 위한 실용적인 구조입니다.
