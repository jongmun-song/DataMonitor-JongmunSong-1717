#include "../Refresh.h"

#include <gtest/gtest.h>

#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>

namespace
{
    // Manually parses "YYYY-MM-DD HH:MM:SS" into its numeric components.
    // Returns false if the string does not match the expected shape.
    bool TryParseTimestamp(const std::string& timestamp, int& year, int& month, int& day,
        int& hour, int& minute, int& second)
    {
        static const std::regex kPattern(
            R"(^(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})$)");
        std::smatch match;
        if (!std::regex_match(timestamp, match, kPattern))
        {
            return false;
        }

        year = std::stoi(match[1].str());
        month = std::stoi(match[2].str());
        day = std::stoi(match[3].str());
        hour = std::stoi(match[4].str());
        minute = std::stoi(match[5].str());
        second = std::stoi(match[6].str());
        return true;
    }
}

TEST(RefreshTest, ReturnsStringOfExactlyNineteenCharacters)
{
    const std::string timestamp = CurrentTimestampString();
    EXPECT_EQ(timestamp.size(), 19u);
}

TEST(RefreshTest, MatchesExpectedDateTimeFormat)
{
    const std::string timestamp = CurrentTimestampString();
    static const std::regex kPattern(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$)");
    EXPECT_TRUE(std::regex_match(timestamp, kPattern))
        << "Got: \"" << timestamp << "\"";
}

TEST(RefreshTest, ConsecutiveCallsAlwaysReturnValidFormat)
{
    const std::string first = CurrentTimestampString();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    const std::string second = CurrentTimestampString();

    static const std::regex kPattern(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$)");
    EXPECT_TRUE(std::regex_match(first, kPattern));
    EXPECT_TRUE(std::regex_match(second, kPattern));

    // The second call must not be earlier than the first (monotonic wall clock
    // expectation over a very short interval); string comparison works because
    // the format is zero-padded and lexicographically ordered.
    EXPECT_LE(first, second);
}

TEST(RefreshTest, ParsedComponentsAreWithinPlausibleRanges)
{
    const std::string timestamp = CurrentTimestampString();

    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    ASSERT_TRUE(TryParseTimestamp(timestamp, year, month, day, hour, minute, second))
        << "Got: \"" << timestamp << "\"";

    // Loose bounds: avoid asserting an exact "today" match (system-clock
    // dependent, brittle around midnight/timezone edge cases), but still
    // reject obviously wrong values.
    EXPECT_GE(year, 2024);
    EXPECT_LE(year, 2100);
    EXPECT_GE(month, 1);
    EXPECT_LE(month, 12);
    EXPECT_GE(day, 1);
    EXPECT_LE(day, 31);
    EXPECT_GE(hour, 0);
    EXPECT_LE(hour, 23);
    EXPECT_GE(minute, 0);
    EXPECT_LE(minute, 59);
    EXPECT_GE(second, 0);
    EXPECT_LE(second, 59);
}
