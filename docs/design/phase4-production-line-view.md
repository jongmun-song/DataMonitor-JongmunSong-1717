# Phase 4 설계: 생산라인 조회 (requirements.pdf p.21)

관련 계획: [`docs/PLAN.md`](../PLAN.md#phase-4-생산라인-조회-requirementspdf-p21)
관련 요구사항: [`docs/feature/production-line-view.md`](../feature/production-line-view.md)

## 목표

생산 큐(`ProductionQueueEntry`)를 이용해 현재 처리 중인 항목과 대기 중인 주문(FIFO
순서)을 콘솔에서 조회할 수 있게 한다. Phase 0의 `ProductionQueueDataSource`를
사용한다.

## 조회 로직

```cpp
std::optional<Model::ProductionQueueEntry> FindCurrentlyProducing(
    const std::vector<Model::ProductionQueueEntry>& queue);

std::vector<Model::ProductionQueueEntry> WaitingQueueInFifoOrder(
    const std::vector<Model::ProductionQueueEntry>& queue);
```

- `FindCurrentlyProducing()`: `state == PRODUCING`인 항목 중 `orderId`가 가장 작은
  하나를 반환한다(요구사항 문서상 생산 라인은 단일 라인이므로 동시에 두 건 이상
  PRODUCING 상태가 있을 가능성은 낮지만, 있더라도 먼저 큐에 들어온 것을
  "현재 처리 중"으로 표시한다). `PRODUCING` 상태 항목이 없으면 `std::nullopt`을
  반환한다.
- `WaitingQueueInFifoOrder()`: `state == WAITING`인 항목만 골라 `orderId` 오름차순으로
  정렬해 반환한다(선입선출).
- 이 저장소는 진행률(%)이나 완료 예정 시각을 계산하지 않는다 — `ProductionQueueEntry`에
  저장된 `orderedQuantity`/`shortageQuantity`/`actualProductionQuantity`/
  `totalProductionTime` 값을 그대로 보여줄 뿐이다.

## 콘솔 UI 확장

```cpp
// ConsoleApp.h
private:
    void handleProductionLineView();   // 메뉴 "4. 생산라인 조회" 핸들러
```

`handleProductionLineView()` 흐름:

1. `productionQueue_.reload()` 호출.
2. `FindCurrentlyProducing()` 결과가 있으면 주문번호(`orderId`) / 시료 ID
   (`sampleId`) / 주문량(`orderedQuantity`) / 부족분(`shortageQuantity`) / 실생산량
   (`actualProductionQuantity`) / 총 생산 시간(`totalProductionTime`)을 출력한다.
   없으면 "현재 처리 중인 항목 없음"을 출력한다.
3. `WaitingQueueInFifoOrder()` 결과를 순서 번호와 함께 표 형태로 출력한다(비어
   있으면 "대기 중인 주문 없음").

## 메뉴 갱신

```
1. 시료 관리 조회
2. 주문 리스트 조회
3. 모니터링 요약
4. 생산라인 조회
0. 종료
```

## 완료 기준 (실행 확인)

- 대기열에 여러 건이 있을 때 `orderId` 오름차순(FIFO)으로 출력됨을 확인한다.
- `PRODUCING` 상태 항목이 없는 상태에서도 크래시 없이 "현재 처리 중인 항목 없음"이
  출력됨을 확인한다.
- 대기열이 완전히 비어 있는 상태에서도 크래시 없이 처리됨을 확인한다.
