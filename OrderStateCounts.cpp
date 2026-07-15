#include "OrderStateCounts.h"

using DataPersistence::Model::OrderState;

OrderStateCounts CountOrdersByState(const std::vector<DataPersistence::Model::Order>& orders)
{
    OrderStateCounts counts;

    for (const auto& order : orders)
    {
        switch (order.state)
        {
        case OrderState::RESERVED:
            ++counts.reserved;
            break;
        case OrderState::CONFIRMED:
            ++counts.confirmed;
            break;
        case OrderState::PRODUCING:
            ++counts.producing;
            break;
        case OrderState::RELEASE:
            ++counts.release;
            break;
        case OrderState::REJECTED:
            // Not a valid order - excluded from the monitoring summary.
            break;
        }
    }

    return counts;
}
