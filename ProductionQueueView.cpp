#include "ProductionQueueView.h"

#include <algorithm>
#include <iterator>

using DataPersistence::Model::ProductionQueueEntry;
using DataPersistence::Model::ProductionState;

std::optional<ProductionQueueEntry> FindCurrentlyProducing(
    const std::vector<ProductionQueueEntry>& queue)
{
    std::optional<ProductionQueueEntry> currentlyProducing;

    for (const auto& entry : queue)
    {
        if (entry.state != ProductionState::PRODUCING)
        {
            continue;
        }

        if (!currentlyProducing.has_value() || entry.orderId < currentlyProducing->orderId)
        {
            currentlyProducing = entry;
        }
    }

    return currentlyProducing;
}

std::vector<ProductionQueueEntry> WaitingQueueInFifoOrder(
    const std::vector<ProductionQueueEntry>& queue)
{
    std::vector<ProductionQueueEntry> waiting;
    std::copy_if(queue.begin(), queue.end(), std::back_inserter(waiting),
        [](const ProductionQueueEntry& entry)
        {
            return entry.state == ProductionState::WAITING;
        });

    std::sort(waiting.begin(), waiting.end(),
        [](const ProductionQueueEntry& lhs, const ProductionQueueEntry& rhs)
        {
            return lhs.orderId < rhs.orderId;
        });

    return waiting;
}
