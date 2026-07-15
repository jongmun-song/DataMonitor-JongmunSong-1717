#pragma once

// Reusable order-state filter (see docs/design/phase2-order-list-view.md).
//
// Extracted out of ConsoleApp so that Phase 3 (per-state order counts) and
// Phase 5 (CONFIRMED-only release-candidates view) can reuse the same
// filtering logic instead of re-implementing it.

#include <optional>
#include <vector>

#include "dataModel/Order.h"

// Returns the subset of `orders` whose state matches `state`. If `state` is
// std::nullopt, returns all orders unfiltered.
std::vector<DataPersistence::Model::Order> FilterOrdersByState(
    const std::vector<DataPersistence::Model::Order>& orders,
    std::optional<DataPersistence::Model::OrderState> state);
