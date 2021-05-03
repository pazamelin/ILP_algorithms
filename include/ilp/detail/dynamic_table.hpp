#include "ilp/detail/debug.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace ilp::detail
{
    struct dynamic_table_t
    {
        struct path_t
        {
            cvector <int_t> x;
            int_t distance = 0;
        };

        using paths_block = std::unordered_map<VertexDescriptor, path_t>;

        struct entry_t
        {
            path_t path_to;
            paths_block paths_from;
        };

        using entries_block = std::unordered_map<VertexDescriptor, entry_t>;

        template <typename Path>
        bool upd_to(VertexDescriptor v, int_t level, Path&& new_path)
        {
            bool has_updated = false;

            auto& current_path = data[level][v].path_to;
            if (current_path.distance < new_path.distance)
            {
                has_updated = true;
                current_path = std::forward<Path>(new_path);
            }

            return has_updated;
        }

        template <typename Path>
        bool upd_from(VertexDescriptor from, VertexDescriptor to, int_t level, Path&& new_path)
        {
            bool has_updated = false;

            auto& current_path = data[level][from].paths_from[to];
            if (current_path.distance < new_path.distance)
            {
                has_updated = true;
                current_path = std::forward<Path>(new_path);
            }

            return has_updated;
        }

        std::vector<entries_block> data;
    };

} // namespace ilp::detail