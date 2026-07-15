#include "SampleDataSource.h"

#include <fstream>

#include <nlohmann/json.hpp>

SampleDataSource::SampleDataSource(std::filesystem::path jsonPath)
    : jsonPath_(std::move(jsonPath))
{
}

void SampleDataSource::reload()
{
    sampleList_.clear();

    if (!std::filesystem::exists(jsonPath_))
    {
        return;
    }

    std::ifstream file(jsonPath_);
    const nlohmann::json json = nlohmann::json::parse(file);
    sampleList_ = json.get<std::vector<DataPersistence::Model::Sample>>();
}

const std::vector<DataPersistence::Model::Sample>& SampleDataSource::all() const
{
    return sampleList_;
}
