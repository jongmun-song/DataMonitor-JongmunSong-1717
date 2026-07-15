# Phase 3 설계: 모니터링 요약 (requirements.pdf p.19)

관련 계획: [`docs/PLAN.md`](../PLAN.md#phase-3-모니터링-요약-requirementspdf-p19)
관련 요구사항: [`docs/feature/monitoring-summary.md`](../feature/monitoring-summary.md)

## 목표

상태별 주문 건수(REJECTED 제외)와 시료별 재고 현황(여유/부족/고갈, 잔여율)을
요약해서 보여준다. Phase 1의 `SampleDataSource`, Phase 2의 `OrderDataSource` /
`FilterOrdersByState()`를 재사용한다.

## 주문량 확인 — 상태별 건수 집계

```cpp
struct OrderStateCounts
{
    int reserved = 0;
    int confirmed = 0;
    int producing = 0;
    int release = 0;
    // REJECTED는 의도적으로 필드가 없다 — 집계 대상이 아니다.
};

OrderStateCounts CountOrdersByState(const std::vector<Model::Order>& orders);
```

- `CountOrdersByState()`는 `phase2-order-list-view.md`의 `FilterOrdersByState()`를
  RESERVED/CONFIRMED/PRODUCING/RELEASE 각각에 대해 호출한 뒤 `size()`를 센 것과
  동일한 결과를 낸다(내부적으로 재사용해도 되고, 한 번의 순회로 직접 세어도 된다).
- REJECTED 상태 주문은 세지 않는다(요구사항 p.8, p.18 — "REJECTED는 유효한 주문이
  아니므로 모니터링에서 제외").

## 재고량 확인 — 임계치 판정 및 잔여율

요구사항 문서(p.18-19)는 여유/부족/고갈 3단계 상태와 잔여율(%)을 보여줘야 한다고만
명시하고, 그 경계가 되는 정확한 수치는 규정하지 않는다. `Sample` 모델에는 "안전
재고"나 "최대 용량" 같은 별도 필드가 없으므로, 이 PoC는 다음 기준을 자체적으로
정의하여 사용한다(추후 `SampleOrderSystem`이 더 정교한 기준— 예: 시료별 안전재고
필드 추가 —으로 대체할 수 있다).

```cpp
constexpr int kLowStockThreshold = 100;   // 이 값 미만이면 "부족"

enum class StockLevel { Depleted, Low, Sufficient };  // 고갈 / 부족 / 여유

StockLevel ClassifyStockLevel(int stockQuantity);
double CalculateRemainingRatioPercent(int stockQuantity);
```

- `stockQuantity == 0` → `Depleted`("고갈"), 잔여율 0%.
- `0 < stockQuantity < kLowStockThreshold` → `Low`("부족"), 잔여율 =
  `stockQuantity * 100.0 / kLowStockThreshold` (0% 초과 100% 미만).
- `stockQuantity >= kLowStockThreshold` → `Sufficient`("여유"), 잔여율 =
  `std::min(100.0, stockQuantity * 100.0 / kLowStockThreshold)`.
- `kLowStockThreshold`는 이름 있는 상수로 한 곳에 정의하고, 하드코딩된 매직 넘버로
  여러 곳에 흩어놓지 않는다.

## 콘솔 UI 확장

```cpp
// ConsoleApp.h
private:
    void handleMonitoringSummary();       // 메뉴 "3. 모니터링 요약" 핸들러
    void printOrderStateCounts();         // 하위 "1. 주문량 확인"
    void printStockLevels();              // 하위 "2. 재고량 확인"
```

`handleMonitoringSummary()`는 하위 메뉴(`1. 주문량 확인`, `2. 재고량 확인`, `0. 뒤로
가기`)를 출력하고 선택에 따라 `printOrderStateCounts()` 또는 `printStockLevels()`를
호출한다. 두 핸들러 모두 진입 시 해당 데이터소스를 `reload()`한다.

`printOrderStateCounts()`: RESERVED/CONFIRMED/PRODUCING/RELEASE 각 건수를 한 줄씩
출력한다(p.19 예시 UI 참고).

`printStockLevels()`: 각 시료의 이름, 재고, 상태(여유/부족/고갈), 잔여율(%)을 표
형태로 출력한다(p.19 예시 UI의 진행률 바는 선택 구현 — 텍스트 퍼센트만으로도
충분하다).

## 메뉴 갱신

```
1. 시료 관리 조회
2. 주문 리스트 조회
3. 모니터링 요약
0. 종료
```

## 완료 기준 (실행 확인)

- REJECTED 상태 주문이 섞여 있어도 "주문량 확인"의 합계에는 반영되지 않음을 확인한다.
- `stockQuantity`가 0인 시료가 "고갈"로, `kLowStockThreshold` 미만인 시료가
  "부족"으로, 그 이상인 시료가 "여유"로 표시됨을 확인한다.
- 잔여율이 0%~100% 범위를 벗어나지 않음을 확인한다.
