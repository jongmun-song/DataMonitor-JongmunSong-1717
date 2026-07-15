# 데이터 모델 & 데이터소스

## 개요

이 저장소가 조회할 데이터 모델과, 그 데이터를 JSON 파일로부터 읽기 전용으로
불러오는 데이터소스 계층. 이후 모든 조회 기능(`sample-view.md`,
`order-list-view.md`, `monitoring-summary.md`, `production-line-view.md`,
`release-candidates-view.md`)이 이 계층 위에서 동작한다.

## 데이터 모델 — `dataModel/`

데이터 모델은 이미 이 저장소의 `dataModel/` 폴더에 제공되어 있다: `Sample.h`,
`Order.h`, `ProductionQueueEntry.h`. 새로 정의하지 않고 그대로 사용한다.

| 파일 | 네임스페이스 | 필드 |
|---|---|---|
| `dataModel/Sample.h` | `DataPersistence::Model` | `id`, `name`, `averageProductionTimePerUnit`, `yieldRatio`, `stockQuantity` |
| `dataModel/Order.h` | `DataPersistence::Model` | `id`, `sampleId`, `customerName`, `orderedQuantity`, `state`(`OrderState`: RESERVED/CONFIRMED/PRODUCING/RELEASE/REJECTED) |
| `dataModel/ProductionQueueEntry.h` | `DataPersistence::Model` | `orderId`, `sampleId`, `orderedQuantity`, `shortageQuantity`, `actualProductionQuantity`, `totalProductionTime`, `state`(`ProductionState`: WAITING/PRODUCING/CONFIRMED) |

세 파일 모두 네임스페이스가 `DataPersistence::Model`로 되어 있다(파일명만 이
저장소의 `DataPersistence` 프로젝트가 아니라 `DataMonitor`에 속한 것일 뿐, 원본
네임스페이스는 그대로다). 이 저장소의 코드는 `DataMonitor::Model` 같은 새 네임스페이스로
감싸거나 재정의하지 않고, `DataPersistence::Model::Sample`/`Order`/
`ProductionQueueEntry`를 그대로 `using` 하거나 정규화된 이름으로 참조한다.

각 구조체는 `NLOHMANN_DEFINE_TYPE_INTRUSIVE`/`NLOHMANN_JSON_SERIALIZE_ENUM`로 JSON과
상호 변환할 수 있으며, 비즈니스 규칙(재고 증감 가드, 상태 전이 검증, 생산량/시간
계산 등)을 갖지 않는 순수 데이터 구조체다.

## 모니터링 대상 데이터 — `storedData/`

이 도구가 실시간으로 조회하는 대상은 이 저장소의 `storedData/` 폴더에 이미
포함되어 있는 JSON 파일들이다.

| 파일 | 대응 모델 |
|---|---|
| `storedData/samples.json` | `dataModel/Sample.h`의 `Sample` 배열 |
| `storedData/orders.json` | `dataModel/Order.h`의 `Order` 배열 |
| `storedData/production_queue.json` | `dataModel/ProductionQueueEntry.h`의 `ProductionQueueEntry` 배열 |

이 파일들의 스키마는 `../DataPersistence`가 실제로 읽고 쓰는(또는 앞으로 읽고 쓸)
JSON 형식과 동일하다 — 최상위가 객체 배열(`[ { ... }, { ... } ]`)이고, 각 객체의
키는 구조체 필드명과 정확히 같다. 따라서 `../DataPersistence`를 실제로 실행해 만든
`samples.json`으로 `storedData/samples.json`을 교체하면, 곧바로 실제 저장된 데이터를
조회할 수 있다.

## 데이터소스 동작

- 각 모델별로 읽기 전용 데이터소스(`SampleDataSource`, `OrderDataSource`,
  `ProductionQueueDataSource`)를 둔다. 각각의 책임은 "JSON 파일 → 메모리 목록"
  변환뿐이며, 값을 수정하거나 저장하는 기능은 갖지 않는다(이 저장소는 조회 전용
  도구이며, 데이터 변경은 `SampleOrderSystem`/`DataPersistence`의 책임이다).
- `reload()`를 호출할 때마다 지정된 JSON 파일을 처음부터 다시 읽어 메모리 목록을
  교체한다. 이전 조회 결과를 내부에 캐싱해 stale 상태로 남겨두지 않는다(실시간성
  요구사항, `live-refresh.md` 참고).
- 파일이 존재하지 않으면 빈 목록으로 처리한다(예외를 던지지 않는다). 파일은 있으나
  파싱에 실패하면 예외를 상위로 전달한다.
- 각 데이터소스는 생성자로 JSON 파일 경로를 받는다(하드코딩하지 않는다). 기본값은
  `storedData/samples.json` 등 위 표의 경로이며, 실행 파일의 작업 디렉터리(working
  directory)가 프로젝트 루트를 가리키도록 구성한다(`docs/design/phase0-foundation.md`
  "작업 디렉터리" 절 참고).

## 라이브러리 연동 — nlohmann/json

`dataModel/*.h`가 `#include <nlohmann/json.hpp>`에 의존하므로, 이 저장소에도
`../DataPersistence/external/nlohmann/json.hpp`와 동일한 방식으로 헤더 온리
라이브러리를 vendored로 포함하고, 프로젝트의 `AdditionalIncludeDirectories`에
등록한다(`docs/design/phase0-foundation.md` 참고). 별도의 패키지 관리자(vcpkg/NuGet)는
필요하지 않다.

## 관련 기능

- `sample-view.md`, `order-list-view.md`, `monitoring-summary.md`,
  `production-line-view.md`, `release-candidates-view.md`: 이 데이터소스가 제공하는
  목록을 조회·요약해서 보여주는 화면들.
- `live-refresh.md`: 매 조회 시점마다 `reload()`를 호출해 최신 상태를 반영하는 실시간
  갱신 규칙.
