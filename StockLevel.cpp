#include "StockLevel.h"

#include <algorithm>

StockLevel ClassifyStockLevel(int stockQuantity)
{
    if (stockQuantity == 0)
    {
        return StockLevel::Depleted;
    }
    if (stockQuantity < kLowStockThreshold)
    {
        return StockLevel::Low;
    }
    return StockLevel::Sufficient;
}

double CalculateRemainingRatioPercent(int stockQuantity)
{
    if (stockQuantity == 0)
    {
        return 0.0;
    }

    const double ratio = stockQuantity * 100.0 / kLowStockThreshold;
    if (stockQuantity >= kLowStockThreshold)
    {
        return std::min(100.0, ratio);
    }
    return ratio;
}

std::string ToKoreanString(StockLevel level)
{
    switch (level)
    {
    case StockLevel::Depleted:
        return "고갈";
    case StockLevel::Low:
        return "부족";
    case StockLevel::Sufficient:
        return "여유";
    }
    return "알수없음";
}
