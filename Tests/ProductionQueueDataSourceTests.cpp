#include "../DataSource/ProductionQueueDataSource.h"

#include <gtest/gtest.h>

#include "TestTempFile.h"

using DataPersistence::Model::ProductionQueueEntry;
using DataPersistence::Model::ProductionState;

// storedData/production_queue.json is a fixture provided by the repository
// (see docs/feature/data-source.md); it currently has 2 entries: one
// PRODUCING (orderId 1) and one WAITING (orderId 2).
TEST(ProductionQueueDataSourceTest, ReloadLoadsExistingStoredProductionQueueJson)
{
    ProductionQueueDataSource dataSource(std::filesystem::path("storedData") / "production_queue.json");

    dataSource.reload();

    const auto& entries = dataSource.all();
    ASSERT_EQ(entries.size(), 2u);
    EXPECT_EQ(entries[0].orderId, 1);
    EXPECT_EQ(entries[0].sampleId, 1);
    EXPECT_EQ(entries[0].orderedQuantity, 150);
    EXPECT_EQ(entries[0].shortageQuantity, 150);
    EXPECT_EQ(entries[0].actualProductionQuantity, 190);
    EXPECT_DOUBLE_EQ(entries[0].totalProductionTime, 1143.0);
    EXPECT_EQ(entries[0].state, ProductionState::PRODUCING);
    EXPECT_EQ(entries[1].orderId, 2);
    EXPECT_EQ(entries[1].state, ProductionState::WAITING);
}

TEST(ProductionQueueDataSourceTest, ReloadOnMissingFileYieldsEmptyListWithoutThrowing)
{
    ProductionQueueDataSource dataSource(std::filesystem::path("storedData") / "does_not_exist.json");

    EXPECT_NO_THROW(dataSource.reload());
    EXPECT_TRUE(dataSource.all().empty());
}

TEST(ProductionQueueDataSourceTest, ReloadOnMalformedJsonPropagatesParseError)
{
    TestTempFile brokenJson("production_queue_broken.json", "not json at all");
    ProductionQueueDataSource dataSource(brokenJson.path());

    EXPECT_THROW(dataSource.reload(), nlohmann::json::parse_error);
}

TEST(ProductionQueueDataSourceTest, ReloadReplacesPreviousListInsteadOfCaching)
{
    TestTempFile jsonFile("production_queue_live.json", R"([
        { "orderId": 2, "sampleId": 1, "orderedQuantity": 100, "shortageQuantity": 100,
          "actualProductionQuantity": 0, "totalProductionTime": 0.0, "state": "WAITING" },
        { "orderId": 1, "sampleId": 1, "orderedQuantity": 50, "shortageQuantity": 50,
          "actualProductionQuantity": 0, "totalProductionTime": 0.0, "state": "WAITING" }
    ])");
    ProductionQueueDataSource dataSource(jsonFile.path());
    dataSource.reload();
    ASSERT_EQ(dataSource.all().size(), 2u);

    jsonFile.write(R"([
        { "orderId": 3, "sampleId": 2, "orderedQuantity": 10, "shortageQuantity": 0,
          "actualProductionQuantity": 10, "totalProductionTime": 5.0, "state": "CONFIRMED" }
    ])");
    dataSource.reload();

    const auto& entries = dataSource.all();
    ASSERT_EQ(entries.size(), 1u);
    EXPECT_EQ(entries[0].orderId, 3);
    EXPECT_EQ(entries[0].state, ProductionState::CONFIRMED);
}
