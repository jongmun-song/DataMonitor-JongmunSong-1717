#include "../OrderStateCounts.h"

#include <gtest/gtest.h>

using DataPersistence::Model::Order;
using DataPersistence::Model::OrderState;

namespace
{
    Order MakeOrder(int id, OrderState state)
    {
        Order order;
        order.id = id;
        order.sampleId = 1;
        order.customerName = "Customer";
        order.orderedQuantity = 10;
        order.state = state;
        return order;
    }
}

TEST(OrderStateCountsTest, CountsEachStateSeparately)
{
    const std::vector<Order> orders = {
        MakeOrder(1, OrderState::RESERVED),
        MakeOrder(2, OrderState::RESERVED),
        MakeOrder(3, OrderState::CONFIRMED),
        MakeOrder(4, OrderState::PRODUCING),
        MakeOrder(5, OrderState::RELEASE),
    };

    const OrderStateCounts counts = CountOrdersByState(orders);

    EXPECT_EQ(counts.reserved, 2);
    EXPECT_EQ(counts.confirmed, 1);
    EXPECT_EQ(counts.producing, 1);
    EXPECT_EQ(counts.release, 1);
}

TEST(OrderStateCountsTest, ExcludesRejectedOrdersFromAllCounts)
{
    const std::vector<Order> orders = {
        MakeOrder(1, OrderState::RESERVED),
        MakeOrder(2, OrderState::REJECTED),
        MakeOrder(3, OrderState::REJECTED),
    };

    const OrderStateCounts counts = CountOrdersByState(orders);

    EXPECT_EQ(counts.reserved, 1);
    EXPECT_EQ(counts.confirmed, 0);
    EXPECT_EQ(counts.producing, 0);
    EXPECT_EQ(counts.release, 0);
}

TEST(OrderStateCountsTest, EmptyInputYieldsAllZeroCounts)
{
    const std::vector<Order> orders;

    const OrderStateCounts counts = CountOrdersByState(orders);

    EXPECT_EQ(counts.reserved, 0);
    EXPECT_EQ(counts.confirmed, 0);
    EXPECT_EQ(counts.producing, 0);
    EXPECT_EQ(counts.release, 0);
}

TEST(OrderStateCountsTest, AllOrdersInSameStateCountsOnlyThatState)
{
    const std::vector<Order> orders = {
        MakeOrder(1, OrderState::RESERVED),
        MakeOrder(2, OrderState::RESERVED),
        MakeOrder(3, OrderState::RESERVED),
    };

    const OrderStateCounts counts = CountOrdersByState(orders);

    EXPECT_EQ(counts.reserved, 3);
    EXPECT_EQ(counts.confirmed, 0);
    EXPECT_EQ(counts.producing, 0);
    EXPECT_EQ(counts.release, 0);
}
