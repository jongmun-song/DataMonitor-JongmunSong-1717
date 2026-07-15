# Phase 0 설계: 프로젝트 뼈대 + 데이터 모델/데이터소스 연동

관련 계획: [`docs/PLAN.md`](../PLAN.md#phase-0-프로젝트-뼈대--데이터-모델데이터소스-연동)
관련 요구사항: [`docs/feature/data-source.md`](../feature/data-source.md)

## 목표

이후 모든 phase가 공유할 모듈 구조(모델 / 데이터소스 / 콘솔 UI)를 확정한다. 데이터
모델과 더미 데이터는 이미 저장소에 준비되어 있으므로(`dataModel/`, `storedData/`),
Phase 0의 작업은 그 위에 데이터소스/콘솔 UI 뼈대를 세우고 빌드 가능하게 만드는
것이다. 이 시점의 실행 결과물은 조회 메뉴 없이 "메뉴 표시 → 종료"만 동작하는 최소
콘솔 앱이다.

## 이미 준비된 것

| 경로 | 내용 |
|---|---|
| `dataModel/Sample.h`, `dataModel/Order.h`, `dataModel/ProductionQueueEntry.h` | 데이터 모델(구조체) 정의. `DataPersistence::Model` 네임스페이스, JSON 변환 매크로 포함. 수정하지 않고 그대로 사용한다 |
| `storedData/samples.json`, `storedData/orders.json`, `storedData/production_queue.json` | 단독 실행/시연용 더미 JSON 데이터. `../DataPersistence`가 실제로 읽고 쓰는 형식과 동일한 스키마(`docs/feature/data-source.md` 참고) |

## Phase 0에서 새로 만들 것

| 파일 | 책임 |
|---|---|
| `external/nlohmann/json.hpp` | `dataModel/*.h`가 의존하는 헤더 온리 JSON 라이브러리(vendored) |
| `DataSource/SampleDataSource.h`/`.cpp` | `storedData/samples.json` 읽기, 메모리 상 목록 관리 |
| `DataSource/OrderDataSource.h`/`.cpp` | `storedData/orders.json` 읽기, 메모리 상 목록 관리 |
| `DataSource/ProductionQueueDataSource.h`/`.cpp` | `storedData/production_queue.json` 읽기, 메모리 상 목록 관리 |
| `ConsoleApp.h`/`.cpp` | 메뉴 출력, 사용자 입력 처리, 각 조회 핸들러 호출 |
| `main.cpp` | 엔트리 포인트: 데이터소스 생성, `ConsoleApp` 구동 |

각 파일은 `DataMonitor.vcxproj`(및 `.vcxproj.filters`)에 등록한다. `dataModel/`,
`storedData/`도 아직 프로젝트에 등록되어 있지 않다면 함께 등록한다(`dataModel/*.h`는
헤더 파일 필터, `storedData/*.json`은 리소스 파일 등으로 분류).

## 라이브러리 연동 방식

- `../DataPersistence/external/nlohmann/json.hpp`를 그대로 복사해
  `DataMonitor/external/nlohmann/json.hpp`에 vendored로 포함한다.
- `DataMonitor.vcxproj`의 `AdditionalIncludeDirectories`에 `$(ProjectDir)external`과
  `$(ProjectDir)`(또는 `dataModel/`이 있는 위치)을 등록해, `#include
  <nlohmann/json.hpp>`와 `#include "dataModel/Sample.h"`가 모두 해석되도록 한다.

## 데이터 모델 사용법

```cpp
// DataSource/SampleDataSource.h
#include "../dataModel/Sample.h"

class SampleDataSource
{
public:
    explicit SampleDataSource(std::filesystem::path jsonPath);

    // JSON 파일을 처음부터 다시 읽어 목록을 교체한다. 파일이 없으면 빈 목록이
    // 된다. 파일은 있으나 파싱 오류면 예외(nlohmann::json::parse_error)가
    // 상위로 전달된다. 내부에 이전 결과를 남겨두지 않는다(캐시 없음).
    void reload();

    const std::vector<DataPersistence::Model::Sample>& all() const;

private:
    std::filesystem::path jsonPath_;
    std::vector<DataPersistence::Model::Sample> sampleList_;
};
```

- `dataModel/Sample.h`가 이미 `DataPersistence::Model` 네임스페이스로 되어 있으므로,
  이 저장소의 코드에서도 그 네임스페이스를 그대로 쓴다(새 네임스페이스로 감싸지
  않는다). 반복적으로 쓰기 번거로우면 `DataSource/*.h`에서
  `namespace Model = DataPersistence::Model;` 같은 별칭을 둘 수 있다.
- `OrderDataSource`, `ProductionQueueDataSource`도 동일한 인터페이스 형태
  (`reload()`/`all()`)를 갖는다. 세 데이터소스는 서로 독립적이며 저장/수정 메서드를
  갖지 않는다(읽기 전용).

## 콘솔 UI 골격

```cpp
// ConsoleApp.h
class ConsoleApp
{
public:
    ConsoleApp(SampleDataSource& samples, OrderDataSource& orders,
               ProductionQueueDataSource& productionQueue);
    void run();   // 메뉴 출력 -> 입력 처리 -> 종료 시까지 반복

private:
    void printMenu() const;
    SampleDataSource& samples_;
    OrderDataSource& orders_;
    ProductionQueueDataSource& productionQueue_;
};
```

- Phase 0의 메뉴 항목은 `0. 종료` 하나뿐이다. 이후 phase마다 항목이 하나씩 늘어난다.

## 작업 디렉터리

`storedData/samples.json` 등은 프로젝트 루트 기준 상대 경로이므로, 실행 시 작업
디렉터리(working directory)가 프로젝트 루트를 가리켜야 기본 경로가 정상적으로
해석된다.

- Visual Studio 디버거로 실행할 때는 `DataMonitor.vcxproj.user`에
  `<LocalDebuggerWorkingDirectory>$(ProjectDir)</LocalDebuggerWorkingDirectory>`를
  Debug/Release·x86/x64 각 구성에 추가한다.
- 빌드된 `.exe`를 `x64\Debug\` 등에서 직접 실행하는 경우를 위해, 각 데이터소스
  생성자는 경로를 인자로 받으므로 `main.cpp`에서 실행 파일 위치와 무관하게 항상
  프로젝트 루트의 `storedData/`를 가리키도록 절대 경로로 바꾸거나, 사용자가 실행 시
  인자로 경로를 넘길 수 있게 해도 된다(둘 다 선택 구현 — 최소한 VS 디버거 실행
  경로가 항상 동작해야 한다).

## 완료 기준 (실행 확인)

- 빌드: `msbuild DataMonitor.vcxproj /p:Configuration=Debug /p:Platform=x64` 성공.
- 실행: `storedData/samples.json` 등 더미 JSON을 정상적으로 로드한 뒤 메뉴가
  출력되고, `0` 입력 시 크래시 없이 정상 종료된다.
