# PLAN

`docs/PRD.md`와 `docs/feature/*.md`에 정의된 요구사항을 구현하기 위한 phase별 계획.
각 phase는 manager → developer → code-reviewer → tester 파이프라인(`.claude/agents/`)을
거쳐 완료하며, **phase가 끝날 때마다 빌드·실행 가능한 콘솔 애플리케이션**을 산출물로
남긴다. 다음 phase는 이전 phase가 남긴 실행 가능한 상태 위에 기능을 덧붙이는 방식으로
진행한다.

각 phase가 다루는 화면은 `../ref/requirements.pdf`의 실제 예시 UI 페이지에 대응한다.

| Phase | 화면 | requirements.pdf |
|---|---|---|
| 0 | 프로젝트 뼈대 + 데이터 모델/데이터소스 | Chapter 3 [미션1], `DataPersistence/Model` 참고 |
| 1 | 시료 관리 조회 | p.13 |
| 2 | 주문 리스트 조회 | p.17 |
| 3 | 모니터링 요약 | p.19 |
| 4 | 생산라인 조회 | p.21 |
| 5 | 출고 가능 조회 | p.23 |
| 6 | 실시간 갱신 통합 | p.10-11, p.19 |

## Phase 0: 프로젝트 뼈대 + 데이터 모델/데이터소스 연동

- 목표: 이미 저장소에 준비된 데이터 모델(`dataModel/Sample.h`, `dataModel/Order.h`,
  `dataModel/ProductionQueueEntry.h`)과 더미 데이터(`storedData/samples.json`,
  `storedData/orders.json`, `storedData/production_queue.json`) 위에, 이를 읽기
  전용으로 불러오는 데이터소스 계층과 콘솔 뼈대를 세운다. `dataModel/*.h`가
  `nlohmann/json.hpp`에 의존하므로 해당 라이브러리를 vendored로 연동한다.
- 참고 문서: `docs/feature/data-source.md`
- 산출물: `external/nlohmann/json.hpp`(vendored), 각 모델별 읽기 전용 데이터소스
  (`SampleDataSource`, `OrderDataSource`, `ProductionQueueDataSource`), `ConsoleApp`/
  `main.cpp` 뼈대.
- **실행 가능한 결과물**: 프로그램을 실행하면 메뉴(종료 옵션만 포함)가 뜨고,
  `storedData/`의 더미 JSON을 정상적으로 읽어들인 뒤 크래시 없이 종료된다. 조회 메뉴
  항목은 아직 없어도 된다.
- 완료 기준: `msbuild`로 빌드가 성공하고, 실행 시 메뉴 → 종료 흐름이 동작하며, 더미
  JSON 파일 로드가 예외 없이 이루어진다.

## Phase 1: 시료 관리 조회 (requirements.pdf p.13)

- 목표: 등록된 시료 전체 목록과 현재 재고 수량을 조회한다.
- 참고 문서: `docs/feature/sample-view.md`
- **실행 가능한 결과물**: 메뉴에 "시료 관리 조회" 항목이 추가되어, 실행 중 선택하면
  더미 데이터의 시료 목록(ID/이름/평균 생산시간/수율/재고)이 표 형태로 출력된다.
- 완료 기준: 등록된 시료가 있을 때/없을 때(빈 목록) 모두 실제 실행으로 확인된다.

## Phase 2: 주문 리스트 조회 (requirements.pdf p.17)

- 목표: 주문 목록을 상태(RESERVED/CONFIRMED/PRODUCING/RELEASE/REJECTED) 기준으로
  검색·조회한다.
- 참고 문서: `docs/feature/order-list-view.md`
- **실행 가능한 결과물**: 메뉴에 "주문 리스트 조회" 항목이 추가되어, 상태를 선택(전체
  또는 특정 상태)하면 해당 조건에 맞는 주문(주문번호/고객명/시료/수량/상태)이
  출력된다.
- 완료 기준: 특정 상태 선택 시 그 상태의 주문만 나오는지, 결과가 없는 상태를 선택했을
  때 빈 목록으로 처리되는지 실제 실행으로 확인된다.

## Phase 3: 모니터링 요약 (requirements.pdf p.19)

- 목표: 상태별 주문 건수(REJECTED 제외)와 시료별 재고 현황(여유/부족/고갈, 잔여율)을
  요약해서 보여준다.
- 참고 문서: `docs/feature/monitoring-summary.md`
- **실행 가능한 결과물**: 메뉴에 "모니터링 요약" 항목이 추가되고, 그 하위에 "주문량
  확인"/"재고량 확인" 두 화면이 각각 실행된다.
- 완료 기준: REJECTED 주문이 상태별 건수 집계에서 제외되는지, 재고 수량에 따라
  여유/부족/고갈 상태와 잔여율이 올바르게 표시되는지 실제 실행으로 확인된다.

## Phase 4: 생산라인 조회 (requirements.pdf p.21)

- 목표: 생산 큐(`ProductionQueueEntry`)를 이용해 현재 처리 중인 항목과 대기 중인
  주문(FIFO 순서)을 조회한다.
- 참고 문서: `docs/feature/production-line-view.md`
- **실행 가능한 결과물**: 메뉴에 "생산라인 조회" 항목이 추가되어, 현재 처리 중인 항목과
  대기열(FIFO 순서) 목록이 출력된다.
- 완료 기준: 대기열이 `orderId` 오름차순(선입선출)으로 출력되는지, 대기열이 비어 있을
  때도 크래시 없이 처리되는지 실제 실행으로 확인된다.

## Phase 5: 출고 가능 조회 (requirements.pdf p.23)

- 목표: CONFIRMED 상태(출고 대기 중)의 주문 목록을 조회한다.
- 참고 문서: `docs/feature/release-candidates-view.md`
- **실행 가능한 결과물**: 메뉴에 "출고 가능 조회" 항목이 추가되어, CONFIRMED 상태
  주문(주문번호/고객명/시료/수량)만 출력된다.
- 완료 기준: CONFIRMED 상태 주문만 나오는지, 대상이 없을 때 빈 목록으로 처리되는지
  실제 실행으로 확인된다.

## Phase 6: 실시간 갱신 통합

- 목표: Phase 1~5의 모든 조회 화면에 공통으로 실시간 갱신(조회 시각 표기, 갱신 명령
  또는 자동 주기 갱신)을 적용하고, 메인 메뉴에 전체 요약 현황을 표시한다.
- 참고 문서: `docs/feature/live-refresh.md`
- **실행 가능한 결과물**: 메인 메뉴 진입 시 등록 시료 수/총 재고/전체 주문 수 등
  요약이 표시되고, 각 조회 화면에서 조회 시각과 함께 최신 데이터가 표시된다. 더미
  JSON 파일을 실행 중 외부에서 수정한 뒤 갱신하면 변경된 값이 반영된다.
- 완료 기준: 모든 조회 화면에 조회 시각이 표기되고, 데이터를 캐싱하지 않고 매 갱신마다
  JSON 파일을 다시 읽어 최신 상태를 반영하는지 실제 실행으로 확인된다.

## Phase 진행 방식

- 각 phase는 순서대로 진행하되, 이전 phase가 개발자(developer) → 코드리뷰어
  (code-reviewer) → 테스터(tester) 검증을 모두 통과해 실행 가능한 상태로 확인된 뒤
  다음 phase로 넘어간다.
- phase 완료 기준(빌드 성공 + 실제 실행 확인)을 만족하지 못하면 해당 phase 내에서
  재작업하며, 다음 phase로 넘어가지 않는다.
- 지금 어느 phase까지 진행됐는지는 이 문서와 `docs/design/`의 대응 phase 문서, 그리고
  실제로 빌드·실행해본 결과로 확인한다.
