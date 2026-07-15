# Phase 6 설계: 실시간 갱신 통합

관련 계획: [`docs/PLAN.md`](../PLAN.md#phase-6-실시간-갱신-통합)
관련 요구사항: [`docs/feature/live-refresh.md`](../feature/live-refresh.md)

## 목표

Phase 1~5의 모든 조회 화면에 공통 실시간 갱신(조회 시각 표기, 수동 갱신 명령)을
적용하고, 메인 메뉴에 전체 요약 현황을 표시한다. 이 phase가 끝나면 요구사항
문서(p.10-11, p.19)의 메인 메뉴/모니터링 화면 수준의 완성된 콘솔 도구가 된다.

## 공통 갱신 유틸리티

```cpp
// Refresh.h
std::string CurrentTimestampString();   // 예: "2026-04-16 09:32:15"
```

- Phase 1~5의 각 핸들러(`handleSampleView`, `handleOrderListView`,
  `handleMonitoringSummary`의 하위 핸들러, `handleProductionLineView`,
  `handleReleaseCandidatesView`)는 데이터를 출력하기 직전에 `CurrentTimestampString()`
  결과를 함께 표시하도록 수정한다.
- 각 화면 하단에는 "갱신하려면 r, 뒤로 가려면 0을 입력하세요" 안내를 추가하고, `r`
  입력 시 해당 데이터소스를 다시 `reload()`한 뒤 같은 화면을 다시 그린다(자동 주기
  갱신은 선택 구현 사항으로 남겨둔다 — 구현 시 켜고 끌 수 있는 명령을 함께 제공한다).

## 메인 메뉴 요약 현황

```cpp
// ConsoleApp.h
private:
    void printMainMenuSummary();   // 등록 시료 종수 / 총 재고 / 전체 주문 건수
```

`printMainMenuSummary()` 흐름:

1. `samples_.reload()`, `orders_.reload()` 호출.
2. 등록 시료 종수(`samples_.all().size()`), 총 재고(모든 시료의 `stockQuantity` 합),
   전체 주문 건수(`orders_.all().size()`)를 조회 시각과 함께 출력한다(p.11 예시 UI
   참고).
3. 그 아래 메뉴 항목(1~5, 0)을 출력한다.

## 최종 메뉴

```
반도체 시료 생산주문관리 시스템 — 데이터 모니터링
조회 시각: 2026-04-16 09:32:15
등록 시료 5종   총 재고 1,630ea   전체 주문 12건

1. 시료 관리 조회
2. 주문 리스트 조회
3. 모니터링 요약
4. 생산라인 조회
5. 출고 가능 조회
0. 종료
```

## 완료 기준 (실행 확인)

- 메인 메뉴 진입 시 요약 현황과 조회 시각이 표시됨을 확인한다.
- Phase 1~5의 각 화면에서 조회 시각이 표시되고, `r` 입력 시 재조회 후 화면이 다시
  그려짐을 확인한다.
- `samples.json`(또는 `orders.json`)을 실행 중 외부 편집기로 수정한 뒤 해당 화면에서
  갱신하면, 프로그램을 재시작하지 않고도 변경된 값이 반영됨을 확인한다(캐시 없는
  실시간 조회의 핵심 검증).
