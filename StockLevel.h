#pragma once

// Stock-level classification for the monitoring summary (see
// docs/design/phase3-monitoring-summary.md, requirements.pdf p.18-19).
//
// requirements.pdf does not specify an exact threshold between "sufficient"
// and "low" stock, so this PoC defines its own (see the design doc). A
// future consumer (e.g. SampleOrderSystem) may replace this with a
// per-sample safe-stock field.

#include <string>

// Below this quantity a sample is considered "low" stock (부족). At or above
// it, the sample is considered "sufficient" (여유). Zero is always
// "depleted" (고갈) regardless of this threshold.
constexpr int kLowStockThreshold = 100;

enum class StockLevel
{
    Depleted,   // 고갈: stockQuantity == 0
    Low,        // 부족: 0 < stockQuantity < kLowStockThreshold
    Sufficient  // 여유: stockQuantity >= kLowStockThreshold
};

// Classifies `stockQuantity` into a StockLevel using kLowStockThreshold.
StockLevel ClassifyStockLevel(int stockQuantity);

// Returns the remaining-stock ratio as a percentage in [0, 100].
//
// - stockQuantity == 0 -> 0%
// - 0 < stockQuantity < kLowStockThreshold -> stockQuantity * 100.0 / kLowStockThreshold
// - stockQuantity >= kLowStockThreshold -> min(100.0, stockQuantity * 100.0 / kLowStockThreshold)
double CalculateRemainingRatioPercent(int stockQuantity);

// Human-readable Korean label for a StockLevel ("여유"/"부족"/"고갈").
std::string ToKoreanString(StockLevel level);
