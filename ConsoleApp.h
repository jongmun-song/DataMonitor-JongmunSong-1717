#pragma once

// Console UI shell for the DataMonitor tool (see docs/design/phase0-foundation.md).
//
// Displays a menu, dispatches the user's choice to a handler, and repeats
// until the user chooses to quit. Every query screen reloads its data
// source(s) on entry and on every manual refresh (see
// docs/design/phase6-live-refresh.md) so the tool never shows stale, cached
// data.

#include <functional>
#include <optional>
#include <string>

#include "DataSource/OrderDataSource.h"
#include "DataSource/ProductionQueueDataSource.h"
#include "DataSource/SampleDataSource.h"
#include "dataModel/Order.h"

class ConsoleApp
{
public:
    ConsoleApp(SampleDataSource& samples, OrderDataSource& orders,
        ProductionQueueDataSource& productionQueue);

    // Prints the main-menu summary and the menu, reads a choice, dispatches
    // it, and repeats until the user quits.
    void run();

private:
    // Prints the system-wide summary shown above the main menu (registered
    // sample count, total stock, total order count) plus the query
    // timestamp (see docs/design/phase6-live-refresh.md). Reloads samples_
    // and orders_ so the summary is never stale.
    void printMainMenuSummary();

    void printMenu() const;

    // Reads a single menu-choice line from stdin. Returns false if input has
    // been exhausted (e.g. EOF), in which case run() treats it as a request
    // to quit.
    bool readChoice(std::string& choice) const;

    // Dispatches a menu choice. Returns false when the user chose to quit.
    bool handleChoice(const std::string& choice);

    // Repeatedly invokes `render` (which is expected to reload its data
    // source(s), print the query timestamp, and print the screen body),
    // then prompts the user to enter 'r' to refresh (reload + redraw) or
    // '0' to return to the main menu. Any input other than 'r'/'R' is
    // treated as "go back", matching the "뒤로 가려면 0" prompt.
    void runRefreshLoop(const std::function<void()>& render);

    // Menu "1. 시료 관리 조회": reload+refresh loop around the sample table
    // (see docs/design/phase1-sample-view.md).
    void handleSampleView();
    void printSampleTable();

    // Menu "2. 주문 리스트 조회": prompts for a state filter once, then runs
    // a reload+refresh loop that keeps re-querying with that same filter
    // (see docs/design/phase2-order-list-view.md).
    void handleOrderListView();
    void printOrderTable(std::optional<DataPersistence::Model::OrderState> stateFilter);

    // Menu "3. 모니터링 요약": shows a submenu (주문량 확인/재고량 확인) and
    // dispatches to the matching handler (see
    // docs/design/phase3-monitoring-summary.md).
    void handleMonitoringSummary();

    // Submenu "1. 주문량 확인": reload+refresh loop showing RESERVED/
    // CONFIRMED/PRODUCING/RELEASE counts (REJECTED is excluded).
    void printOrderStateCounts();
    void printOrderStateCountsBody();

    // Submenu "2. 재고량 확인": reload+refresh loop showing each sample's
    // stock quantity, level (여유/부족/고갈), and remaining ratio (%).
    void printStockLevels();
    void printStockLevelsBody();

    // Menu "4. 생산라인 조회": reload+refresh loop showing the
    // currently-producing entry plus the waiting queue in FIFO order (see
    // docs/design/phase4-production-line-view.md).
    void handleProductionLineView();
    void printProductionLineBody();

    // Menu "5. 출고 가능 조회": reload+refresh loop showing CONFIRMED orders
    // (i.e. orders awaiting release) as a table (see
    // docs/design/phase5-release-candidates-view.md).
    void handleReleaseCandidatesView();
    void printReleaseCandidatesBody();

    SampleDataSource& samples_;
    OrderDataSource& orders_;
    ProductionQueueDataSource& productionQueue_;
};
