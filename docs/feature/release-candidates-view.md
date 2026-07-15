# 출고 가능 조회 (requirements.pdf p.23)

## 개요

출고 대기 중(`CONFIRMED` 상태)인 주문 목록을 조회하는 화면. 요구사항 문서의 "출고
처리 - 예시 UI 화면"(p.23)의 "출고 가능 주문" 목록에 대응한다.

## 동작

- `order-list-view.md`와 동일한 `OrderDataSource` 조회 경로를 사용하되, 상태 필터를
  `CONFIRMED`로 고정한다(사용자가 상태를 선택할 필요가 없는 전용 화면).
- 조회 전 매번 `reload()`하여 최신 CONFIRMED 목록을 가져온다.
- 결과를 주문번호/고객명/시료/수량 순서의 표로 출력한다(p.23 예시 UI 참고, 상태
  컬럼은 모두 CONFIRMED로 동일하므로 생략 가능).
- CONFIRMED 상태 주문이 없으면 빈 목록임을 알린다(예외를 던지지 않는다).
- 이 화면은 조회 전용이며, 실제 출고 처리(상태를 RELEASE로 전환하는 것)는 수행하지
  않는다 — 그것은 `SampleOrderSystem`의 "출고 처리" 기능(p.22-23) 책임이다.

## 관련 기능

- `data-source.md`, `order-list-view.md`: 이 화면이 재사용하는 조회/필터링 로직의
  출처.
- `live-refresh.md`: 매 조회 시점의 데이터 재조회 및 조회 시각 표기.
