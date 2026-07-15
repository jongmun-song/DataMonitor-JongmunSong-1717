# 생산라인 조회 (requirements.pdf p.21)

## 개요

생산 큐(`ProductionQueueEntry`)를 이용해 현재 처리 중인 생산 항목과, 대기 중인
주문(FIFO 순서)을 조회하는 화면. 요구사항 문서의 "생산라인"(p.21, FIFO 방식,
단일 생산 라인)에 대응한다.

## 동작

- `data-source.md`의 `ProductionQueueDataSource`로 생산 큐 목록을 조회 전 매번
  `reload()`한다.
- **현재 처리 중**: `state == PRODUCING`인 항목 중 큐에 먼저 들어온(=`orderId`가 가장
  작은) 항목 하나를 "현재 처리 중"으로 표시한다. 주문번호, 시료, 주문량, 부족분
  (`shortageQuantity`), 실생산량(`actualProductionQuantity`), 총 생산 시간
  (`totalProductionTime`)을 함께 보여준다(p.21 예시 UI 참고). `PRODUCING` 상태
  항목이 없으면 "현재 처리 중인 항목 없음"으로 표시한다.
- **대기 중인 주문**: `state == WAITING`인 항목을 `orderId` 오름차순(선입선출, FIFO)으로
  정렬하여 목록으로 보여준다. 각 행에는 주문번호, 시료, 주문량, 부족분, 실생산량을
  표시한다.
- 이 화면은 생산 진행률이나 완료 예정 시각을 자체적으로 계산/시뮬레이션하지 않는다
  (`totalProductionTime` 등 저장된 값을 그대로 표시할 뿐이다) — 실제 생산 진행 관리는
  `SampleOrderSystem`의 책임이다.

## 관련 기능

- `data-source.md`: 생산 큐 목록을 불러올 때 사용.
- `live-refresh.md`: 매 조회 시점의 데이터 재조회 및 조회 시각 표기.
