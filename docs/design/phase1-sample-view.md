# Phase 1 설계: 시료 관리 조회 (requirements.pdf p.13)

관련 계획: [`docs/PLAN.md`](../PLAN.md#phase-1-시료-관리-조회-requirementspdf-p13)
관련 요구사항: [`docs/feature/sample-view.md`](../feature/sample-view.md)

## 목표

등록된 시료 전체 목록과 현재 재고 수량을 콘솔에서 조회할 수 있게 한다. Phase 0의
`SampleDataSource` 위에 첫 조회 화면을 추가한다.

## 콘솔 UI 확장

```cpp
// ConsoleApp.h
private:
    void handleSampleView();   // 메뉴 "1. 시료 관리 조회" 핸들러
```

`handleSampleView()` 흐름:

1. `samples_.reload()`를 호출해 `samples.json`을 다시 읽는다(캐시 없음, `docs/feature/live-refresh.md` 참고).
2. `samples_.all()`이 비어 있으면 "등록된 시료가 없습니다"를 출력한다.
3. 비어 있지 않으면 각 시료를 한 줄씩 표 형태로 출력한다: ID / 이름 / 평균
   생산시간(`averageProductionTimePerUnit`) / 수율(`yieldRatio`) / 현재 재고
   (`stockQuantity`).

## 메뉴 갱신

```
1. 시료 관리 조회
0. 종료
```

## 완료 기준 (실행 확인)

- `samples.json`에 2건 이상의 더미 데이터가 있을 때, 메뉴 1 선택 시 모두 표로
  출력됨을 확인한다.
- `samples.json`이 비어 있거나 존재하지 않을 때도 크래시 없이 "등록된 시료가
  없습니다"가 출력됨을 확인한다.
