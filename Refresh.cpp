#include "Refresh.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string CurrentTimestampString()
{
    const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    const std::time_t nowTimeT = std::chrono::system_clock::to_time_t(now);

    std::tm localTm{};
    localtime_s(&localTm, &nowTimeT);

    std::ostringstream out;
    out << std::put_time(&localTm, "%Y-%m-%d %H:%M:%S");
    return out.str();
}
