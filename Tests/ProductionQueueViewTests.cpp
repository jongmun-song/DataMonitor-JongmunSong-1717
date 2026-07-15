#include "../ProductionQueueView.h"

#include <gtest/gtest.h>

using DataPersistence::Model::ProductionQueueEntry;
using DataPersistence::Model::ProductionState;

namespace
{
    ProductionQueueEntry MakeEntry(int orderId, ProductionState state)
    {
        ProductionQueueEntry entry;
        entry.orderId = orderId;
        entry.sampleId = 1;
        entry.orderedQuantity = 10;
        entry.shortageQuantity = 5;
        entry.actualProductionQuantity = 12;
        entry.totalProductionTime = 100.0;
        entry.state = state;
        return entry;
    }
}

TEST(ProductionQueueViewTest, FindCurrentlyProducingReturnsNulloptWhenNoneProducing)
{
    const std::vector<ProductionQueueEntry> queue = {
        MakeEntry(1, ProductionState::WAITING),
        MakeEntry(2, ProductionState::CONFIRMED),
    };

    EXPECT_FALSE(FindCurrentlyProducing(queue).has_value());
}

TEST(ProductionQueueViewTest, FindCurrentlyProducingReturnsNulloptOnEmptyQueue)
{
    const std::vector<ProductionQueueEntry> emptyQueue;

    EXPECT_FALSE(FindCurrentlyProducing(emptyQueue).has_value());
}

TEST(ProductionQueueViewTest, FindCurrentlyProducingPicksSmallestOrderIdAmongProducing)
{
    const std::vector<ProductionQueueEntry> queue = {
        MakeEntry(5, ProductionState::PRODUCING),
        MakeEntry(2, ProductionState::PRODUCING),
        MakeEntry(1, ProductionState::WAITING),
    };

    const std::optional<ProductionQueueEntry> result = FindCurrentlyProducing(queue);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->orderId, 2);
}

TEST(ProductionQueueViewTest, WaitingQueueInFifoOrderSortsByOrderIdAscending)
{
    const std::vector<ProductionQueueEntry> queue = {
        MakeEntry(3, ProductionState::WAITING),
        MakeEntry(1, ProductionState::PRODUCING),
        MakeEntry(2, ProductionState::WAITING),
        MakeEntry(4, ProductionState::CONFIRMED),
    };

    const std::vector<ProductionQueueEntry> result = WaitingQueueInFifoOrder(queue);

    ASSERT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0].orderId, 2);
    EXPECT_EQ(result[1].orderId, 3);
}

TEST(ProductionQueueViewTest, WaitingQueueInFifoOrderReturnsEmptyWhenNoneWaiting)
{
    const std::vector<ProductionQueueEntry> queue = {
        MakeEntry(1, ProductionState::PRODUCING),
        MakeEntry(2, ProductionState::CONFIRMED),
    };

    EXPECT_TRUE(WaitingQueueInFifoOrder(queue).empty());
}

TEST(ProductionQueueViewTest, WaitingQueueInFifoOrderReturnsEmptyOnEmptyQueue)
{
    const std::vector<ProductionQueueEntry> emptyQueue;

    EXPECT_TRUE(WaitingQueueInFifoOrder(emptyQueue).empty());
}

TEST(ProductionQueueViewTest, FindCurrentlyProducingIgnoresConfirmedEvenWithSmallerOrderId)
{
    const std::vector<ProductionQueueEntry> queue = {
        MakeEntry(1, ProductionState::CONFIRMED),
        MakeEntry(9, ProductionState::PRODUCING),
        MakeEntry(4, ProductionState::WAITING),
    };

    const std::optional<ProductionQueueEntry> result = FindCurrentlyProducing(queue);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->orderId, 9);
}

TEST(ProductionQueueViewTest, WaitingQueueInFifoOrderDoesNotMutateInputVector)
{
    const std::vector<ProductionQueueEntry> queue = {
        MakeEntry(3, ProductionState::WAITING),
        MakeEntry(1, ProductionState::PRODUCING),
        MakeEntry(2, ProductionState::WAITING),
    };
    const std::vector<ProductionQueueEntry> original = queue;

    const std::vector<ProductionQueueEntry> result = WaitingQueueInFifoOrder(queue);

    ASSERT_EQ(queue.size(), original.size());
    for (size_t i = 0; i < queue.size(); ++i)
    {
        EXPECT_EQ(queue[i].orderId, original[i].orderId);
        EXPECT_EQ(queue[i].state, original[i].state);
    }
    ASSERT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0].orderId, 2);
    EXPECT_EQ(result[1].orderId, 3);
}
