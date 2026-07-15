#pragma once

// Reusable production-queue view logic (see
// docs/design/phase4-production-line-view.md).
//
// Extracted out of ConsoleApp so the FIFO/producing-lookup logic can be
// tested independently, following the pattern of OrderFilter (Phase 2).

#include <optional>
#include <vector>

#include "dataModel/ProductionQueueEntry.h"

// Returns the PRODUCING entry with the smallest orderId (the one that
// entered the queue first), or std::nullopt if no entry is PRODUCING.
std::optional<DataPersistence::Model::ProductionQueueEntry> FindCurrentlyProducing(
    const std::vector<DataPersistence::Model::ProductionQueueEntry>& queue);

// Returns the WAITING entries sorted by orderId ascending (FIFO order).
std::vector<DataPersistence::Model::ProductionQueueEntry> WaitingQueueInFifoOrder(
    const std::vector<DataPersistence::Model::ProductionQueueEntry>& queue);
