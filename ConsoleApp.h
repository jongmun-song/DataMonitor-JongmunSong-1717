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

    SampleDataSource& samples_;
    OrderDataSource& orders_;
    ProductionQueueDataSource& productionQueue_;
};
