# 주문 리스트 조회 (requirements.pdf p.17)

## 개요

주문(`Order`) 목록을 상태 기준으로 검색·조회하는 화면. 요구사항 문서의 "주문
승인/거절 - 예시 UI 화면"(p.17)에서 보여주는 "승인 대기 중인 예약 목록"(RESERVED
상태 주문 목록)을 일반화하여, RESERVED뿐 아니라 임의의 상태(전체 포함)로 조회할 수
있게 한다.

## 동작

- 사용자로부터 조회할 상태를 입력받는다: 전체 / RESERVED / CONFIRMED / PRODUCING /
  RELEASE / REJECTED 중 선택.
- `data-source.md`의 `OrderDataSource`로 주문 목록을 조회 전 매번 `reload()`한 뒤,
  선택된 상태와 일치하는 주문만 필터링한다("전체" 선택 시 필터링 없이 전부 표시).
- 결과를 주문번호/고객명/시료/수량/상태 순서의 표로 출력한다(p.17 예시 UI 참고,
  화면 레이아웃 자체는 자유).
- 조건에 맞는 주문이 없으면 빈 목록임을 알린다(예외를 던지지 않는다).

## 관련 기능

- `data-source.md`: 주문 목록을 불러올 때 사용.
- `monitoring-summary.md`: 이 화면의 상태별 필터링 로직을 재사용해 상태별 건수를
  집계한다.
- `release-candidates-view.md`: CONFIRMED 상태만 조회하는 특화된 화면으로, 이 기능의
  상태 필터 조회를 CONFIRMED로 고정한 형태다.
