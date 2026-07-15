#pragma once

// Read-only data source for Order records (see docs/feature/data-source.md).
//
// Loads storedData/orders.json into memory. Does not cache stale results:
// every reload() call re-reads the JSON file from disk, replacing the
// in-memory list. Does not offer any mutation/save capability - this
// project is a read-only monitoring tool.

#include <filesystem>
#include <vector>

#include "../dataModel/Order.h"

class OrderDataSource
{
public:
    explicit OrderDataSource(std::filesystem::path jsonPath);

    // Re-reads the JSON file from scratch and replaces the in-memory list.
    // If the file does not exist, the list becomes empty. If the file exists
    // but fails to parse, the exception (nlohmann::json::parse_error) is
    // propagated to the caller.
    void reload();

    const std::vector<DataPersistence::Model::Order>& all() const;

private:
    std::filesystem::path jsonPath_;
    std::vector<DataPersistence::Model::Order> orderList_;
};
