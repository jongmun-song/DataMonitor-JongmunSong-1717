#include "ConsoleApp.h"

#include <iomanip>
#include <ios>
#include <iostream>
#include <optional>
#include <string>

#include "OrderFilter.h"
#include "OrderStateCounts.h"
#include "ProductionQueueView.h"
#include "StockLevel.h"

namespace
{
    using DataPersistence::Model::OrderState;

    // Human-readable label for an order state (see
    // docs/design/phase2-order-list-view.md).
    std::string ToString(OrderState state)
    {
        switch (state)
        {
        case OrderState::RESERVED:
            return "RESERVED";
        case OrderState::CONFIRMED:
            return "CONFIRMED";
        case OrderState::PRODUCING:
            return "PRODUCING";
        case OrderState::RELEASE:
            return "RELEASE";
        case OrderState::REJECTED:
            return "REJECTED";
        }
        return "UNKNOWN";
    }

    // Maps the menu digit entered by the user to a state filter. Returns
    // std::nullopt for "0" (all states) or an unrecognized digit.
    std::optional<OrderState> ToOrderStateFilter(const std::string& choice)
    {
        if (choice == "1")
        {
            return OrderState::RESERVED;
        }
        if (choice == "2")
        {
            return OrderState::CONFIRMED;
        }
        if (choice == "3")
        {
            return OrderState::PRODUCING;
        }
        if (choice == "4")
        {
            return OrderState::RELEASE;
        }
        if (choice == "5")
        {
            return OrderState::REJECTED;
        }
        return std::nullopt;
    }
}

ConsoleApp::ConsoleApp(SampleDataSource& samples, OrderDataSource& orders,
    ProductionQueueDataSource& productionQueue)
    : samples_(samples)
    , orders_(orders)
    , productionQueue_(productionQueue)
{
}

void ConsoleApp::run()
{
    bool keepRunning = true;
    while (keepRunning)
    {
        printMenu();

        std::string choice;
        if (!readChoice(choice))
        {
            break;
        }

        keepRunning = handleChoice(choice);
    }
}

void ConsoleApp::printMenu() const
{
    std::cout << "\n=== DataMonitor ===\n";
    std::cout << "1. 시료 관리 조회\n";
    std::cout << "2. 주문 리스트 조회\n";
    std::cout << "3. 모니터링 요약\n";
    std::cout << "4. 생산라인 조회\n";
    std::cout << "0. 종료\n";
    std::cout << "선택: ";
}

bool ConsoleApp::readChoice(std::string& choice) const
{
    return static_cast<bool>(std::getline(std::cin, choice));
}

bool ConsoleApp::handleChoice(const std::string& choice)
{
    if (choice == "0")
    {
        return false;
    }

    if (choice == "1")
    {
        handleSampleView();
        return true;
    }

    if (choice == "2")
    {
        handleOrderListView();
        return true;
    }

    if (choice == "3")
    {
        handleMonitoringSummary();
        return true;
    }

    if (choice == "4")
    {
        handleProductionLineView();
        return true;
    }

    std::cout << "알 수 없는 메뉴입니다: " << choice << "\n";
    return true;
}

void ConsoleApp::handleSampleView()
{
    samples_.reload();
    const auto& sampleList = samples_.all();

    std::cout << "\n--- 시료 관리 조회 ---\n";

    if (sampleList.empty())
    {
        std::cout << "등록된 시료가 없습니다\n";
        return;
    }

    std::cout << std::left
        << std::setw(6) << "ID"
        << std::setw(20) << "이름"
        << std::setw(16) << "평균생산시간"
        << std::setw(10) << "수율"
        << std::setw(10) << "현재재고"
        << "\n";

    for (const auto& sample : sampleList)
    {
        std::cout << std::left
            << std::setw(6) << sample.id
            << std::setw(20) << sample.name
            << std::setw(16) << sample.averageProductionTimePerUnit
            << std::setw(10) << sample.yieldRatio
            << std::setw(10) << sample.stockQuantity
            << "\n";
    }
}

void ConsoleApp::handleOrderListView()
{
    std::cout << "\n--- 주문 리스트 조회 ---\n";
    std::cout << "0. 전체  1. RESERVED  2. CONFIRMED  3. PRODUCING  4. RELEASE  5. REJECTED\n";
    std::cout << "조회할 상태 선택: ";

    std::string choice;
    if (!readChoice(choice))
    {
        return;
    }

    const std::optional<OrderState> stateFilter = ToOrderStateFilter(choice);

    orders_.reload();
    const std::vector<DataPersistence::Model::Order> filteredOrders =
        FilterOrdersByState(orders_.all(), stateFilter);

    if (filteredOrders.empty())
    {
        std::cout << "조건에 맞는 주문이 없습니다\n";
        return;
    }

    std::cout << std::left
        << std::setw(10) << "주문번호"
        << std::setw(16) << "고객명"
        << std::setw(10) << "시료ID"
        << std::setw(10) << "수량"
        << std::setw(12) << "상태"
        << "\n";

    for (const auto& order : filteredOrders)
    {
        std::cout << std::left
            << std::setw(10) << order.id
            << std::setw(16) << order.customerName
            << std::setw(10) << order.sampleId
            << std::setw(10) << order.orderedQuantity
            << std::setw(12) << ToString(order.state)
            << "\n";
    }
}

void ConsoleApp::handleMonitoringSummary()
{
    std::cout << "\n--- 모니터링 요약 ---\n";
    std::cout << "1. 주문량 확인  2. 재고량 확인  0. 뒤로가기\n";
    std::cout << "선택: ";

    std::string choice;
    if (!readChoice(choice))
    {
        return;
    }

    if (choice == "1")
    {
        printOrderStateCounts();
        return;
    }

    if (choice == "2")
    {
        printStockLevels();
        return;
    }

    if (choice != "0")
    {
        std::cout << "알 수 없는 메뉴입니다: " << choice << "\n";
    }
}

void ConsoleApp::printOrderStateCounts()
{
    orders_.reload();
    const OrderStateCounts counts = CountOrdersByState(orders_.all());

    std::cout << "\n--- 주문량 확인 (REJECTED 제외) ---\n";
    std::cout << "RESERVED : " << counts.reserved << "\n";
    std::cout << "CONFIRMED: " << counts.confirmed << "\n";
    std::cout << "PRODUCING: " << counts.producing << "\n";
    std::cout << "RELEASE  : " << counts.release << "\n";
}

void ConsoleApp::printStockLevels()
{
    samples_.reload();
    const auto& sampleList = samples_.all();

    std::cout << "\n--- 재고량 확인 ---\n";

    if (sampleList.empty())
    {
        std::cout << "등록된 시료가 없습니다\n";
        return;
    }

    std::cout << std::left
        << std::setw(20) << "이름"
        << std::setw(10) << "재고"
        << std::setw(10) << "상태"
        << std::setw(10) << "잔여율"
        << "\n";

    for (const auto& sample : sampleList)
    {
        const StockLevel level = ClassifyStockLevel(sample.stockQuantity);
        const double remainingRatioPercent = CalculateRemainingRatioPercent(sample.stockQuantity);

        std::cout << std::left
            << std::setw(20) << sample.name
            << std::setw(10) << sample.stockQuantity
            << std::setw(10) << ToKoreanString(level)
            << std::fixed << std::setprecision(1) << remainingRatioPercent << "%"
            << "\n";
    }
}

void ConsoleApp::handleProductionLineView()
{
    productionQueue_.reload();
    const auto& queue = productionQueue_.all();

    std::cout << "\n--- 생산라인 조회 ---\n";

    std::cout << "\n[현재 처리 중]\n";
    const std::optional<DataPersistence::Model::ProductionQueueEntry> currentlyProducing =
        FindCurrentlyProducing(queue);

    if (!currentlyProducing.has_value())
    {
        std::cout << "현재 처리 중인 항목 없음\n";
    }
    else
    {
        std::cout << "주문번호: " << currentlyProducing->orderId << "\n";
        std::cout << "시료ID  : " << currentlyProducing->sampleId << "\n";
        std::cout << "주문량  : " << currentlyProducing->orderedQuantity << "\n";
        std::cout << "부족분  : " << currentlyProducing->shortageQuantity << "\n";
        std::cout << "실생산량: " << currentlyProducing->actualProductionQuantity << "\n";
        std::cout << "총생산시간: " << currentlyProducing->totalProductionTime << "\n";
    }

    std::cout << "\n[대기 중인 주문 (FIFO)]\n";
    const std::vector<DataPersistence::Model::ProductionQueueEntry> waitingQueue =
        WaitingQueueInFifoOrder(queue);

    if (waitingQueue.empty())
    {
        std::cout << "대기 중인 주문 없음\n";
        return;
    }

    std::cout << std::left
        << std::setw(6) << "순번"
        << std::setw(10) << "주문번호"
        << std::setw(10) << "시료ID"
        << std::setw(10) << "주문량"
        << std::setw(10) << "부족분"
        << std::setw(10) << "실생산량"
        << "\n";

    int order = 1;
    for (const auto& entry : waitingQueue)
    {
        std::cout << std::left
            << std::setw(6) << order
            << std::setw(10) << entry.orderId
            << std::setw(10) << entry.sampleId
            << std::setw(10) << entry.orderedQuantity
            << std::setw(10) << entry.shortageQuantity
            << std::setw(10) << entry.actualProductionQuantity
            << "\n";
        ++order;
    }
}
