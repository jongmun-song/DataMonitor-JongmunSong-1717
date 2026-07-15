# Phase 2 설계: 주문 리스트 조회 (requirements.pdf p.17)

관련 계획: [`docs/PLAN.md`](../PLAN.md#phase-2-주문-리스트-조회-requirementspdf-p17)
관련 요구사항: [`docs/feature/order-list-view.md`](../feature/order-list-view.md)

## 목표

주문 목록을 상태 기준(전체 또는 RESERVED/CONFIRMED/PRODUCING/RELEASE/REJECTED)으로
검색·조회할 수 있게 한다. Phase 0의 `OrderDataSource` 위에 조회 화면을 추가한다.

## 필터링 로직

```cpp
// 예: 자유 함수 또는 OrderListView 클래스의 정적 메서드
std::vector<Model::Order> FilterOrdersByState(
    const std::vector<Model::Order>& orders,
    std::optional<Model::OrderState> state);   // std::nullopt == 전체
```

- `state`가 `std::nullopt`이면 필터링 없이 전체를 반환한다.
- `state`가 값을 가지면 `order.state == *state`인 항목만 반환한다.
- 이 함수는 이후 `docs/design/phase3-monitoring-summary.md`(상태별 건수 집계)와
  `docs/design/phase5-release-candidates-view.md`(CONFIRMED 고정 조회)에서 재사용한다.

## 콘솔 UI 확장

```cpp
// ConsoleApp.h
private:
    void handleOrderListView();   // 메뉴 "2. 주문 리스트 조회" 핸들러
```

`handleOrderListView()` 흐름:

1. 조회할 상태를 입력받는다: `0` 전체, `1` RESERVED, `2` CONFIRMED, `3` PRODUCING,
   `4` RELEASE, `5` REJECTED.
2. `orders_.reload()` 호출 후 `FilterOrdersByState()`로 필터링한다.
3. 결과가 비어 있으면 "조건에 맞는 주문이 없습니다"를 출력한다.
4. 비어 있지 않으면 각 주문을 한 줄씩 표 형태로 출력한다: 주문번호(`id`) / 고객명
   (`customerName`) / 시료 ID(`sampleId`) / 수량(`orderedQuantity`) / 상태(`state`).

## 메뉴 갱신

```
1. 시료 관리 조회
2. 주문 리스트 조회
0. 종료
```

## 완료 기준 (실행 확인)

- `orders.json`에 서로 다른 상태의 주문이 섞여 있을 때, 특정 상태를 선택하면 그
  상태의 주문만 출력됨을 확인한다.
- "전체" 선택 시 REJECTED를 포함한 모든 주문이 출력됨을 확인한다(이 화면은 필터
  기능이므로 REJECTED도 조회는 가능해야 한다 — REJECTED를 집계에서 제외하는 것은
  `docs/design/phase3-monitoring-summary.md`의 책임이다).
- 조건에 맞는 주문이 없는 상태(예: RELEASE 주문이 하나도 없을 때 RELEASE 선택)를
  선택해도 크래시 없이 빈 목록 메시지가 출력됨을 확인한다.
