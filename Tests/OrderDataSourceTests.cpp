#include "../DataSource/OrderDataSource.h"

#include <gtest/gtest.h>

#include "TestTempFile.h"

using DataPersistence::Model::Order;
using DataPersistence::Model::OrderState;

// storedData/orders.json is a fixture provided by the repository (see
// docs/feature/data-source.md); it currently has 3 entries, all RESERVED.
TEST(OrderDataSourceTest, ReloadLoadsExistingStoredOrdersJson)
{
    OrderDataSource dataSource(std::filesystem::path("storedData") / "orders.json");

    dataSource.reload();

    const auto& orders = dataSource.all();
    ASSERT_EQ(orders.size(), 3u);
    EXPECT_EQ(orders[0].id, 1);
    EXPECT_EQ(orders[0].sampleId, 1);
    EXPECT_EQ(orders[0].customerName, "LG");
    EXPECT_EQ(orders[0].orderedQuantity, 300);
    EXPECT_EQ(orders[0].state, OrderState::RESERVED);
}

TEST(OrderDataSourceTest, ReloadOnMissingFileYieldsEmptyListWithoutThrowing)
{
    OrderDataSource dataSource(std::filesystem::path("storedData") / "does_not_exist.json");

    EXPECT_NO_THROW(dataSource.reload());
    EXPECT_TRUE(dataSource.all().empty());
}

TEST(OrderDataSourceTest, ReloadOnMalformedJsonPropagatesParseError)
{
    TestTempFile brokenJson("orders_broken.json", "[ { \"id\": 1, ");
    OrderDataSource dataSource(brokenJson.path());

    EXPECT_THROW(dataSource.reload(), nlohmann::json::parse_error);
}

TEST(OrderDataSourceTest, ReloadReflectsAllOrderStatesIncludingRejected)
{
    TestTempFile jsonFile("orders_states.json", R"([
        { "id": 1, "sampleId": 1, "customerName": "A", "orderedQuantity": 10, "state": "RESERVED" },
        { "id": 2, "sampleId": 1, "customerName": "B", "orderedQuantity": 20, "state": "CONFIRMED" },
        { "id": 3, "sampleId": 1, "customerName": "C", "orderedQuantity": 30, "state": "PRODUCING" },
        { "id": 4, "sampleId": 1, "customerName": "D", "orderedQuantity": 40, "state": "RELEASE" },
        { "id": 5, "sampleId": 1, "customerName": "E", "orderedQuantity": 50, "state": "REJECTED" }
    ])");
    OrderDataSource dataSource(jsonFile.path());

    dataSource.reload();

    const auto& orders = dataSource.all();
    ASSERT_EQ(orders.size(), 5u);
    EXPECT_EQ(orders[0].state, OrderState::RESERVED);
    EXPECT_EQ(orders[1].state, OrderState::CONFIRMED);
    EXPECT_EQ(orders[2].state, OrderState::PRODUCING);
    EXPECT_EQ(orders[3].state, OrderState::RELEASE);
    EXPECT_EQ(orders[4].state, OrderState::REJECTED);
}

TEST(OrderDataSourceTest, ReloadReplacesPreviousListInsteadOfCaching)
{
    TestTempFile jsonFile("orders_live.json", R"([
        { "id": 1, "sampleId": 1, "customerName": "Old", "orderedQuantity": 10, "state": "RESERVED" }
    ])");
    OrderDataSource dataSource(jsonFile.path());
    dataSource.reload();
    ASSERT_EQ(dataSource.all().size(), 1u);
    EXPECT_EQ(dataSource.all()[0].customerName, "Old");

    jsonFile.write(R"([
        { "id": 1, "sampleId": 1, "customerName": "New", "orderedQuantity": 999, "state": "CONFIRMED" }
    ])");
    dataSource.reload();

    ASSERT_EQ(dataSource.all().size(), 1u);
    EXPECT_EQ(dataSource.all()[0].customerName, "New");
    EXPECT_EQ(dataSource.all()[0].orderedQuantity, 999);
    EXPECT_EQ(dataSource.all()[0].state, OrderState::CONFIRMED);
}
