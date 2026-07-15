#include "../DataSource/SampleDataSource.h"

#include <gtest/gtest.h>

#include "TestTempFile.h"

using DataPersistence::Model::Sample;

// storedData/samples.json is a fixture provided by the repository (see
// docs/feature/data-source.md); it currently has 5 entries, the last of
// which has stockQuantity 0 (depleted stock boundary case).
TEST(SampleDataSourceTest, ReloadLoadsExistingStoredSamplesJson)
{
    SampleDataSource dataSource(std::filesystem::path("storedData") / "samples.json");

    dataSource.reload();

    const auto& samples = dataSource.all();
    ASSERT_EQ(samples.size(), 5u);
    EXPECT_EQ(samples[0].id, 1);
    EXPECT_EQ(samples[0].name, "Wafer-8in");
    EXPECT_DOUBLE_EQ(samples[0].averageProductionTimePerUnit, 0.5);
    EXPECT_DOUBLE_EQ(samples[0].yieldRatio, 0.92);
    EXPECT_EQ(samples[0].stockQuantity, 380);
    EXPECT_EQ(samples[4].stockQuantity, 0);
}

TEST(SampleDataSourceTest, ReloadOnMissingFileYieldsEmptyListWithoutThrowing)
{
    SampleDataSource dataSource(std::filesystem::path("storedData") / "does_not_exist.json");

    EXPECT_NO_THROW(dataSource.reload());
    EXPECT_TRUE(dataSource.all().empty());
}

// Phase 1 (docs/design/phase1-sample-view.md) requires handleSampleView() to
// print "등록된 시료가 없습니다" without crashing when the sample list is
// empty. Unlike the missing-file case above, this covers an existing file
// that legitimately contains an empty JSON array ("[]") - e.g. right after
// DataPersistence has been reset - which is a distinct boundary case for
// reload()/all().
TEST(SampleDataSourceTest, ReloadOnEmptyJsonArrayYieldsEmptyList)
{
    TestTempFile emptyArrayJson("samples_empty.json", "[]");
    SampleDataSource dataSource(emptyArrayJson.path());

    EXPECT_NO_THROW(dataSource.reload());
    EXPECT_TRUE(dataSource.all().empty());
}

TEST(SampleDataSourceTest, ReloadOnMalformedJsonPropagatesParseError)
{
    TestTempFile brokenJson("samples_broken.json", "{ this is not valid json ]");
    SampleDataSource dataSource(brokenJson.path());

    EXPECT_THROW(dataSource.reload(), nlohmann::json::parse_error);
}

TEST(SampleDataSourceTest, ReloadReplacesPreviousListInsteadOfCaching)
{
    TestTempFile jsonFile("samples_live.json", R"([
        { "id": 1, "name": "Old", "averageProductionTimePerUnit": 0.1, "yieldRatio": 0.5, "stockQuantity": 10 }
    ])");
    SampleDataSource dataSource(jsonFile.path());
    dataSource.reload();
    ASSERT_EQ(dataSource.all().size(), 1u);
    EXPECT_EQ(dataSource.all()[0].stockQuantity, 10);

    jsonFile.write(R"([
        { "id": 1, "name": "New", "averageProductionTimePerUnit": 0.2, "yieldRatio": 0.6, "stockQuantity": 999 },
        { "id": 2, "name": "Another", "averageProductionTimePerUnit": 0.3, "yieldRatio": 0.7, "stockQuantity": 5 }
    ])");
    dataSource.reload();

    ASSERT_EQ(dataSource.all().size(), 2u);
    EXPECT_EQ(dataSource.all()[0].name, "New");
    EXPECT_EQ(dataSource.all()[0].stockQuantity, 999);
    EXPECT_EQ(dataSource.all()[1].stockQuantity, 5);
}
