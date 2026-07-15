#include "OrderFilter.h"

#include <algorithm>
#include <iterator>

std::vector<DataPersistence::Model::Order> FilterOrdersByState(
    const std::vector<DataPersistence::Model::Order>& orders,
    std::optional<DataPersistence::Model::OrderState> state)
{
    if (!state.has_value())
    {
        return orders;
    }

    std::vector<DataPersistence::Model::Order> filtered;
    std::copy_if(orders.begin(), orders.end(), std::back_inserter(filtered),
        [&state](const DataPersistence::Model::Order& order)
        {
            return order.state == *state;
        });
    return filtered;
}
