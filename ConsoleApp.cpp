#include "ConsoleApp.h"

#include <iomanip>
#include <ios>
#include <iostream>
#include <optional>
#include <string>

#include "OrderFilter.h"

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
