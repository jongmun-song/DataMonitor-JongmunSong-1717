#include "ProductionQueueDataSource.h"

#include <fstream>

#include <nlohmann/json.hpp>

ProductionQueueDataSource::ProductionQueueDataSource(std::filesystem::path jsonPath)
    : jsonPath_(std::move(jsonPath))
{
}

void ProductionQueueDataSource::reload()
{
    entryList_.clear();

    if (!std::filesystem::exists(jsonPath_))
    {
        return;
    }

    std::ifstream file(jsonPath_);
    const nlohmann::json json = nlohmann::json::parse(file);
    entryList_ = json.get<std::vector<DataPersistence::Model::ProductionQueueEntry>>();
}

const std::vector<DataPersistence::Model::ProductionQueueEntry>& ProductionQueueDataSource::all() const
{
    return entryList_;
}
