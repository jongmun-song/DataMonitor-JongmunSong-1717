#pragma once

// Console UI shell for the DataMonitor tool (see docs/design/phase0-foundation.md).
//
// Displays a menu, dispatches the user's choice to a handler, and repeats
// until the user chooses to quit. Phase 0 only has the quit option; later
// phases add one menu item (and one handler) each.

#include <string>

#include "DataSource/OrderDataSource.h"
#include "DataSource/ProductionQueueDataSource.h"
#include "DataSource/SampleDataSource.h"

class ConsoleApp
{
public:
    ConsoleApp(SampleDataSource& samples, OrderDataSource& orders,
        ProductionQueueDataSource& productionQueue);

    // Prints the menu, reads a choice, dispatches it, and repeats until the
    // user quits.
    void run();

private:
    void printMenu() const;

    // Reads a single menu-choice line from stdin. Returns false if input has
    // been exhausted (e.g. EOF), in which case run() treats it as a request
    // to quit.
    bool readChoice(std::string& choice) const;

    // Dispatches a menu choice. Returns false when the user chose to quit.
    bool handleChoice(const std::string& choice);

    // Menu "1. 시료 관리 조회": reloads samples from disk and prints them as a
    // table (see docs/design/phase1-sample-view.md).
    void handleSampleView();

    // Menu "2. 주문 리스트 조회": prompts for a state filter, reloads orders
    // from disk, and prints the matching orders as a table (see
    // docs/design/phase2-order-list-view.md).
    void handleOrderListView();

    // Menu "3. 모니터링 요약": shows a submenu (주문량 확인/재고량 확인) and
    // dispatches to the matching handler (see
    // docs/design/phase3-monitoring-summary.md).
    void handleMonitoringSummary();

    // Submenu "1. 주문량 확인": reloads orders and prints RESERVED/CONFIRMED/
    // PRODUCING/RELEASE counts (REJECTED is excluded).
    void printOrderStateCounts();

    // Submenu "2. 재고량 확인": reloads samples and prints each sample's
    // stock quantity, level (여유/부족/고갈), and remaining ratio (%).
    void printStockLevels();

    // Menu "4. 생산라인 조회": reloads the production queue and prints the
    // currently-producing entry plus the waiting queue in FIFO order (see
    // docs/design/phase4-production-line-view.md).
    void handleProductionLineView();

    // Menu "5. 출고 가능 조회": reloads orders and prints CONFIRMED orders
    // (i.e. orders awaiting release) as a table (see
    // docs/design/phase5-release-candidates-view.md).
    void handleReleaseCandidatesView();

    SampleDataSource& samples_;
    OrderDataSource& orders_;
    ProductionQueueDataSource& productionQueue_;
};
