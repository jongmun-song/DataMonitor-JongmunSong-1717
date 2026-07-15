#pragma once

// Per-state order counts for the monitoring summary (see
// docs/design/phase3-monitoring-summary.md, requirements.pdf p.19).
//
// REJECTED orders are intentionally excluded - they are not valid orders and
// must not be reflected in the monitoring summary (requirements.pdf p.8, p.18).

#include <vector>

#include "dataModel/Order.h"

struct OrderStateCounts
{
    int reserved = 0;
    int confirmed = 0;
    int producing = 0;
    int release = 0;
    // REJECTED has no field on purpose - it is not counted.
};

// Counts `orders` by state, ignoring REJECTED orders entirely.
OrderStateCounts CountOrdersByState(const std::vector<DataPersistence::Model::Order>& orders);
