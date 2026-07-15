#include "../OrderFilter.h"

#include <gtest/gtest.h>

using DataPersistence::Model::Order;
using DataPersistence::Model::OrderState;

namespace
{
    // A fixture mixing all five OrderState values (see
    // docs/design/phase2-order-list-view.md and requirements.pdf p.17).
    std::vector<Order> MakeMixedStateOrders()
    {
        Order reserved;
        reserved.id = 1;
        reserved.sampleId = 1;
        reserved.customerName = "A";
        reserved.orderedQuantity = 10;
        reserved.state = OrderState::RESERVED;

        Order confirmed;
        confirmed.id = 2;
        confirmed.sampleId = 1;
        confirmed.customerName = "B";
        confirmed.orderedQuantity = 20;
        confirmed.state = OrderState::CONFIRMED;

        Order producing;
        producing.id = 3;
        producing.sampleId = 1;
        producing.customerName = "C";
        producing.orderedQuantity = 30;
        producing.state = OrderState::PRODUCING;

        Order release;
        release.id = 4;
        release.sampleId = 1;
        release.customerName = "D";
        release.orderedQuantity = 40;
        release.state = OrderState::RELEASE;

        Order rejected;
        rejected.id = 5;
        rejected.sampleId = 1;
        rejected.customerName = "E";
        rejected.orderedQuantity = 50;
        rejected.state = OrderState::REJECTED;

        return { reserved, confirmed, producing, release, rejected };
    }
}

TEST(OrderFilterTest, NulloptReturnsAllOrdersIncludingRejected)
{
    const std::vector<Order> orders = MakeMixedStateOrders();

    const std::vector<Order> result = FilterOrdersByState(orders, std::nullopt);

    ASSERT_EQ(result.size(), 5u);
    EXPECT_EQ(result[0].state, OrderState::RESERVED);
    EXPECT_EQ(result[1].state, OrderState::CONFIRMED);
    EXPECT_EQ(result[2].state, OrderState::PRODUCING);
    EXPECT_EQ(result[3].state, OrderState::RELEASE);
    EXPECT_EQ(result[4].state, OrderState::REJECTED);
}

TEST(OrderFilterTest, SpecificStateReturnsOnlyMatchingOrders)
{
    const std::vector<Order> orders = MakeMixedStateOrders();

    const std::vector<Order> result = FilterOrdersByState(orders, OrderState::RESERVED);

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].id, 1);
    EXPECT_EQ(result[0].state, OrderState::RESERVED);
}

TEST(OrderFilterTest, StateWithNoMatchesReturnsEmptyVectorWithoutThrowing)
{
    std::vector<Order> orders;
    Order reserved;
    reserved.id = 1;
    reserved.state = OrderState::RESERVED;
    orders.push_back(reserved);

    std::vector<Order> result;
    EXPECT_NO_THROW(result = FilterOrdersByState(orders, OrderState::CONFIRMED));
    EXPECT_TRUE(result.empty());
}

TEST(OrderFilterTest, EmptyInputListReturnsEmptyVectorRegardlessOfFilter)
{
    const std::vector<Order> emptyOrders;

    EXPECT_TRUE(FilterOrdersByState(emptyOrders, std::nullopt).empty());
    EXPECT_TRUE(FilterOrdersByState(emptyOrders, OrderState::RESERVED).empty());
}

TEST(OrderFilterTest, RejectedStateCanBeFilteredExplicitly)
{
    const std::vector<Order> orders = MakeMixedStateOrders();

    const std::vector<Order> result = FilterOrdersByState(orders, OrderState::REJECTED);

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].id, 5);
    EXPECT_EQ(result[0].state, OrderState::REJECTED);
}

// FindReleaseCandidates (see docs/design/phase5-release-candidates-view.md):
// release candidates are exactly the CONFIRMED orders.

TEST(FindReleaseCandidatesTest, ReturnsOnlyConfirmedOrdersFromMixedStates)
{
    const std::vector<Order> orders = MakeMixedStateOrders();

    const std::vector<Order> result = FindReleaseCandidates(orders);

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].id, 2);
    EXPECT_EQ(result[0].state, OrderState::CONFIRMED);
}

TEST(FindReleaseCandidatesTest, ReturnsEmptyVectorWhenNoOrderIsConfirmed)
{
    std::vector<Order> orders;
    Order reserved;
    reserved.id = 1;
    reserved.state = OrderState::RESERVED;
    orders.push_back(reserved);

    Order rejected;
    rejected.id = 2;
    rejected.state = OrderState::REJECTED;
    orders.push_back(rejected);

    std::vector<Order> result;
    EXPECT_NO_THROW(result = FindReleaseCandidates(orders));
    EXPECT_TRUE(result.empty());
}

TEST(FindReleaseCandidatesTest, EmptyInputListReturnsEmptyVector)
{
    const std::vector<Order> emptyOrders;

    EXPECT_TRUE(FindReleaseCandidates(emptyOrders).empty());
}
