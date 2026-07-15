#include "OrderDataSource.h"

#include <fstream>

#include <nlohmann/json.hpp>

OrderDataSource::OrderDataSource(std::filesystem::path jsonPath)
    : jsonPath_(std::move(jsonPath))
{
}

void OrderDataSource::reload()
{
    orderList_.clear();

    if (!std::filesystem::exists(jsonPath_))
    {
        return;
    }

    std::ifstream file(jsonPath_);
    const nlohmann::json json = nlohmann::json::parse(file);
    orderList_ = json.get<std::vector<DataPersistence::Model::Order>>();
}

const std::vector<DataPersistence::Model::Order>& OrderDataSource::all() const
{
    return orderList_;
}
