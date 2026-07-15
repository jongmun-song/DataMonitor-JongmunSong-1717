# Phase 5 설계: 출고 가능 조회 (requirements.pdf p.23)

관련 계획: [`docs/PLAN.md`](../PLAN.md#phase-5-출고-가능-조회-requirementspdf-p23)
관련 요구사항: [`docs/feature/release-candidates-view.md`](../feature/release-candidates-view.md)

## 목표

CONFIRMED 상태(출고 대기 중)인 주문 목록을 조회할 수 있게 한다. Phase 2의
`OrderDataSource` / `FilterOrdersByState()`를 CONFIRMED로 고정해 재사용한다.

## 조회 로직

```cpp
std::vector<Model::Order> FindReleaseCandidates(const std::vector<Model::Order>& orders)
{
    return FilterOrdersByState(orders, Model::OrderState::CONFIRMED);
}
```

- `phase2-order-list-view.md`의 `FilterOrdersByState()`를 그대로 호출하므로 별도의
  필터링 로직을 새로 작성하지 않는다(중복 제거).

## 콘솔 UI 확장

```cpp
// ConsoleApp.h
private:
    void handleReleaseCandidatesView();   // 메뉴 "5. 출고 가능 조회" 핸들러
```

`handleReleaseCandidatesView()` 흐름:

1. `orders_.reload()` 호출.
2. `FindReleaseCandidates()`로 CONFIRMED 주문만 골라낸다.
3. 결과가 비어 있으면 "출고 가능한 주문이 없습니다"를 출력한다.
4. 비어 있지 않으면 주문번호 / 고객명 / 시료 ID / 수량 순서의 표로 출력한다(상태
   컬럼은 모두 CONFIRMED이므로 생략 가능).

## 메뉴 갱신

```
1. 시료 관리 조회
2. 주문 리스트 조회
3. 모니터링 요약
4. 생산라인 조회
5. 출고 가능 조회
0. 종료
```

## 완료 기준 (실행 확인)

- CONFIRMED 상태 주문만 출력되고 다른 상태(RESERVED, PRODUCING 등)는 제외됨을
  확인한다.
- CONFIRMED 상태 주문이 하나도 없을 때도 크래시 없이 빈 목록 메시지가 출력됨을
  확인한다.
