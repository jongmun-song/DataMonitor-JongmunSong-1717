// Entry point for the DataMonitor console tool (see docs/design/phase0-foundation.md).
//
// Creates the three read-only data sources against storedData/*.json, loads
// them once up front, and runs the console menu loop.

#include <filesystem>
#include <iostream>

#include "ConsoleApp.h"
#include "DataSource/OrderDataSource.h"
#include "DataSource/ProductionQueueDataSource.h"
#include "DataSource/SampleDataSource.h"

namespace
{
    // storedData/*.json paths are relative to the project root. Running the
    // built .exe directly from e.g. x64/Debug/ would otherwise fail to find
    // them, so walk up from the current working directory until a
    // storedData/ folder is found (the Visual Studio debugger already sets
    // the working directory to the project root via DataMonitor.vcxproj.user,
    // so this is mainly a convenience for running the .exe directly).
    std::filesystem::path FindStoredDataDir()
    {
        std::filesystem::path dir = std::filesystem::current_path();
        for (int depth = 0; depth < 8; ++depth)
        {
            const std::filesystem::path candidate = dir / "storedData";
            if (std::filesystem::exists(candidate))
            {
                return candidate;
            }

            const std::filesystem::path parent = dir.parent_path();
            if (parent == dir)
            {
                break;
            }
            dir = parent;
        }

        return std::filesystem::current_path() / "storedData";
    }
}

int main()
{
    const std::filesystem::path storedDataDir = FindStoredDataDir();

    SampleDataSource samples(storedDataDir / "samples.json");
    OrderDataSource orders(storedDataDir / "orders.json");
    ProductionQueueDataSource productionQueue(storedDataDir / "production_queue.json");

    try
    {
        samples.reload();
        orders.reload();
        productionQueue.reload();
    }
    catch (const std::exception& e)
    {
        std::cerr << "저장된 데이터를 불러오는 중 오류가 발생했습니다: " << e.what() << "\n";
        return 1;
    }

    ConsoleApp app(samples, orders, productionQueue);
    app.run();

    return 0;
}
