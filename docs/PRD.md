# PRD — DataMonitor

## 1. 배경

`SSemiProductManager` 과제는 "반도체 시료 생산주문관리 시스템"(`SampleOrderSystem`)을 구현하기 전, 4개의 PoC(`ConsoleMVC`, `DataPersistence`, `DataMonitor`, `DummyDataGenerator`)로 핵심 기능을 검증하도록 요구한다(`../ref/requirements.pdf` Chapter 3, [미션1]).

`DataMonitor`는 그중 **"데이터 모니터링 Tool"** PoC로, "현재 저장된 데이터 상태를 콘솔에서 실시간 조회할 수 있는 관리자 도구"를 목표로 한다. 자세한 저장소 역할 구분과 데이터 모델은 `../CLAUDE.md`에 정의되어 있으며, 본 문서는 그 내용을 바탕으로 프로젝트 전체 요구사항을 정리한다.

## 2. 목적

`storedData/`에 담긴 JSON 파일(`samples.json`, `orders.json`, `production_queue.json`) — `../DataPersistence`가 실제로 읽고 쓰는(또는 앞으로 읽고 쓸) 것과 동일한 스키마 — 을 대상으로, 반도체 시료(Sample)/주문(Order)/생산 큐(ProductionQueueEntry)의 현재 상태를 콘솔에서 **실시간으로** 조회·요약할 수 있는 PoC 도구를 만든다. PoC 특성상 완성된 범용 모듈을 한 번에 목표로 하기보다, `docs/PLAN.md`의 phase에 따라 단계별로 콘솔에서 눈으로 확인 가능한 결과물을 쌓아 올린다.

## 3. 범위

### 포함 (In Scope)

- `dataModel/`에 이미 정의된 Sample/Order/ProductionQueueEntry 데이터 모델을 그대로 사용하고, `storedData/`의 동일 스키마 JSON 파일을 읽는 읽기 전용 데이터소스
- **시료 관리 조회**(requirements.pdf p.13) — 시료 목록과 현재 재고
- **주문 리스트 조회**(p.17) — 상태 기준 주문 검색·조회
- **모니터링 요약**(p.19) — 상태별 주문 건수 집계(REJECTED 제외), 재고 여유/부족/고갈 임계치 판정·잔여율
- **생산라인 조회**(p.21) — 현재 처리 중인 생산 항목과 FIFO 대기열
- **출고 가능 조회**(p.23) — CONFIRMED 상태 주문 목록
- 위 모든 조회 화면에 공통 적용되는 **실시간 갱신**(조회 시각 표기, 수동 갱신 명령 이상)과 메인 메뉴 요약 현황

### 제외 (Out of Scope)

- 주문 접수/승인/거절, 생산 실행, 출고 처리 등 데이터를 **변경**하는 업무 기능(`SampleOrderSystem` 담당) — 이 저장소는 조회 전용 도구다
- JSON 파일 자체를 쓰는(Create/Update/Delete) 영속성 구현(`DataPersistence` 저장소 담당) — 이 저장소는 그 산출물을 읽기만 한다
- MVC 골격 자체(`ConsoleMVC` 저장소 담당) — 필요 시 그 콘솔 UI 패턴을 참고할 수 있으나 필수 의존은 아님
- 더미 데이터의 대량/무작위 생성 로직 자체(`DummyDataGenerator` 저장소 담당) — 이 저장소는 `storedData/`에 이미 포함된 소수의 고정 더미 JSON 데이터만 사용한다

Chapter 2의 해당 화면들(pp.13, 17, 19, 21, 23)은 참고 사례가 아니라 **이 저장소가 실제로 구현해야 할 화면**이며, 완료 여부는 `docs/PLAN.md`의 phase별 체크리스트로 확인한다.

## 4. 관련 저장소와의 관계

| 저장소 | 관계 |
|---|---|
| `SampleOrderSystem` | 이 PoC에서 검증한 조회 화면/로직을 참고하여, 전체 시스템의 모니터링 관련 메뉴를 구현할 때 재사용 |
| `DataPersistence` | `dataModel/`(모델)·`storedData/`(더미 JSON)의 원출처. `DataPersistence`가 실제로 생성한 `samples.json`으로 `storedData/samples.json`을 교체하면 실제 데이터를 조회한다(`docs/feature/data-source.md` 참고) |
| `DummyDataGenerator` | 대량 더미 데이터가 필요해지면 이 저장소가 사용하는 것과 같은 `storedData/*.json` 파일에 데이터를 채워 넣는 역할 |

## 5. 기술 스택

- 언어/표준: C++20 (`stdcpp20`)
- 빌드: Visual Studio (`DataMonitor.slnx`, `DataMonitor.vcxproj`), MSBuild, Win32 `Application` (콘솔 서브시스템), x86/x64, Debug/Release
- JSON 파싱: [nlohmann/json](https://github.com/nlohmann/json) (헤더 온리, `dataModel/*.h`가 의존하므로 `DataPersistence/external/nlohmann/json.hpp`와 동일한 방식으로 vendored, Phase 0에서 연동)
- 외부 의존성: gtest(`Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn` 1.8.1.8, NuGet) — 테스트용

## 6. 핵심 설계 원칙

1. **단계별 검증 가능성** — 매 phase마다 콘솔에서 실행해 눈으로 확인할 수 있는 상태를 유지한다(`docs/PLAN.md` 참고).
2. **요구사항 문서 기반** — 추상적인 재사용성보다 `requirements.pdf`에 기술된 실제 화면(p.13/17/19/21/23)을 정확히 구현하는 것을 우선한다.
3. **실제 저장 포맷 준수** — 임의의 추상 데이터가 아니라 `dataModel/`·`storedData/`에 이미 주어진 모델/데이터를 그대로 사용한다(`docs/feature/data-source.md`).
4. **실시간성** — 한 번 조회한 결과를 내부에 캐싱해 stale 상태로 남겨두지 않고, 매 갱신마다 JSON 파일을 다시 읽는다.
5. **교체 가능한 데이터 계층** — `storedData/*.json`이 실제 `DataPersistence` 산출물로 교체되거나 파일 경로가 바뀌어도, 집계/View 코드를 건드리지 않고 데이터소스 계층만 바꿀 수 있도록 데이터 접근을 함수/타입 경계로 분리해둔다.

## 7. 산출물

- `external/nlohmann/json.hpp`(vendored)와 `dataModel/`/`storedData/` 위에 세운 읽기 전용 데이터소스(`SampleDataSource`, `OrderDataSource`, `ProductionQueueDataSource`)
- 5개 조회 화면(시료 관리 / 주문 리스트 / 모니터링 요약 / 생산라인 / 출고 가능)
- 실시간 갱신 공통 유틸리티와 메인 메뉴 요약 현황
- `docs/PLAN.md`, `docs/feature/*.md`, `docs/design/phase0-foundation.md` ~ `phase6-live-refresh.md`
- gtest 기반 테스트

## 8. 완료 기준 (Definition of Done)

- `docs/PLAN.md`의 Phase 0~6이 순서대로 구현되고, 각 phase별로 콘솔 실행 결과가 확인됨
- `storedData/samples.json`을 `DataPersistence`가 실제로 생성한 `samples.json`으로 교체해도 정상적으로 읽혀 조회됨
- 상태별 주문 건수 집계에서 REJECTED가 정상적으로 제외됨
- 재고 수량에 대해 여유/부족/고갈 다단계 임계치 판정과 잔여율 표시가 동작함(`docs/design/phase3-monitoring-summary.md`의 기준)
- 생산라인 대기열이 FIFO(주문 ID 오름차순) 순서로 표시됨
- CONFIRMED 상태 주문만 "출고 가능 조회"에 표시됨
- 콘솔에서 조회 시각과 함께 최신 데이터 상태가 반복 갱신되어 표시됨(수동 갱신 명령 최소 지원)
