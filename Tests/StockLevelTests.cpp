#include "../StockLevel.h"

#include <gtest/gtest.h>

TEST(StockLevelTest, ZeroStockIsDepletedWithZeroRatio)
{
    EXPECT_EQ(ClassifyStockLevel(0), StockLevel::Depleted);
    EXPECT_DOUBLE_EQ(CalculateRemainingRatioPercent(0), 0.0);
    EXPECT_EQ(ToKoreanString(StockLevel::Depleted), "고갈");
}

TEST(StockLevelTest, BelowThresholdIsLowWithRatioUnderOneHundred)
{
    EXPECT_EQ(ClassifyStockLevel(kLowStockThreshold - 1), StockLevel::Low);

    const double ratio = CalculateRemainingRatioPercent(kLowStockThreshold - 1);
    EXPECT_GT(ratio, 0.0);
    EXPECT_LT(ratio, 100.0);
    EXPECT_EQ(ToKoreanString(StockLevel::Low), "부족");
}

TEST(StockLevelTest, AtOrAboveThresholdIsSufficientWithRatioCappedAtOneHundred)
{
    EXPECT_EQ(ClassifyStockLevel(kLowStockThreshold), StockLevel::Sufficient);
    EXPECT_DOUBLE_EQ(CalculateRemainingRatioPercent(kLowStockThreshold), 100.0);

    EXPECT_EQ(ClassifyStockLevel(kLowStockThreshold * 10), StockLevel::Sufficient);
    EXPECT_DOUBLE_EQ(CalculateRemainingRatioPercent(kLowStockThreshold * 10), 100.0);
    EXPECT_EQ(ToKoreanString(StockLevel::Sufficient), "여유");
}

TEST(StockLevelTest, JustAboveZeroIsLow)
{
    EXPECT_EQ(ClassifyStockLevel(1), StockLevel::Low);
    EXPECT_DOUBLE_EQ(CalculateRemainingRatioPercent(1), 1.0);
}

TEST(StockLevelTest, TopOfLowRangeJustBelowThresholdHasExactRatio)
{
    // kLowStockThreshold - 1 = 99: highest value still classified as Low.
    EXPECT_EQ(ClassifyStockLevel(kLowStockThreshold - 1), StockLevel::Low);
    EXPECT_DOUBLE_EQ(CalculateRemainingRatioPercent(kLowStockThreshold - 1), 99.0);
}

TEST(StockLevelTest, JustAboveThresholdIsSufficientWithRatioClampedAtOneHundred)
{
    // kLowStockThreshold + 1 = 101: ratio would be 101% unclamped, must clamp to 100.
    EXPECT_EQ(ClassifyStockLevel(kLowStockThreshold + 1), StockLevel::Sufficient);
    EXPECT_DOUBLE_EQ(CalculateRemainingRatioPercent(kLowStockThreshold + 1), 100.0);
}

TEST(StockLevelTest, NegativeStockQuantityCurrentBehavior)
{
    // requirements.pdf does not define behavior for negative stock (should not
    // occur in practice). This test documents the current implementation's
    // behavior rather than asserting a new requirement: a negative value is
    // neither == 0 nor >= kLowStockThreshold, so it currently falls into Low,
    // and the ratio formula yields a negative percentage (not clamped to 0).
    EXPECT_EQ(ClassifyStockLevel(-10), StockLevel::Low);
    EXPECT_DOUBLE_EQ(CalculateRemainingRatioPercent(-10), -10.0);
}
