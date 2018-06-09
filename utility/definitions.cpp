#include <cstddef>
#include <atomic>
#include <unordered_set>
#include <mutex>

#include "UniqueIdGenerator.hpp"

using namespace utils;

namespace
{
    struct DataU32
    {
        using set_type = std::unordered_set<uint32_t>;
        set_type map;
        uint32_t state = 0;
        std::mutex mutex;
    };
}


template <>
std::unique_ptr<UniqueIdGenerator<uint32_t>> UniqueIdGenerator<uint32_t>::_instance{nullptr};
template <>
void* UniqueIdGenerator<uint32_t>::_data{static_cast<void*>(new DataU32)};
template <>
uint32_t UniqueIdGenerator<uint32_t>::generate_impl(void **_data)
{
    auto data = static_cast<DataU32 *>(*_data);
    uint32_t id = 0;
    {
        std::unique_lock<std::mutex> lock(data->mutex);
        id = data->state++;
        while (data->map.count(id) > 0)
        {
            id = data->state++;
        }
        data->map.insert(id);
    }
    LOG_D("Generated id: %d", id)
    return id;
}
template <>
void UniqueIdGenerator<uint32_t>::free_impl(const uint32_t& id, void **_data)
{
    auto data = static_cast<DataU32 *>(*_data);
    {
        std::unique_lock<std::mutex> lock(data->mutex);
        if (data->map.erase(id) > 0)
        {
            LOG_D("Removed id: %d", id)
        }
    }
}