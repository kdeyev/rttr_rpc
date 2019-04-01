#pragma once

#include <nlohmann/json.hpp>
#include <fifo_map.hpp>

namespace rttr_rpc {
    // A workaround to give to use fifo_map as map, we are just ignoring the 'less' compare
    template <class K, class V, class dummy_compare, class A>
    using my_workaround_fifo_map = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;
    using json                   = nlohmann::basic_json<my_workaround_fifo_map>;
} // namespace rttr_rpc
