#ifndef ILP_JANSEN_ROHWEDDER_HPP
#define ILP_JANSEN_ROHWEDDER_HPP

#include "ilp_task.hpp"
#include "digraph.hpp"
#include "detail/dynamic_table.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adj_list_serialize.hpp>

#include <unordered_set>

namespace ilp
{
    int_t compute_H(const matrix<int_t>& A);

    int_t compute_K(const ilp_task& ilpTask);

    struct DynamicTable
    {
        explicit DynamicTable(const ilp_task& ilpTask);

        using PointId = std::size_t;

        template <typename PointType>
        std::size_t get_point_id(PointType&& point)
        {
            std::size_t result;

            auto it = m_points.find(point);
            if (it != m_points.end())
            {
                result =  it->second;
            }
            else
            {
                result = m_points.size();
                m_points.insert({std::forward<PointType>(point), result});
            }

            return result;
        }

        struct Path
        {
            cvector <int_t> x;
            int_t distance = std::numeric_limits<int_t>::min();
        };

        using PathsBlock = std::unordered_map<PointId, Path>;

        struct Entry
        {
            cvector<int_t> point;
            Path path_to;
            PathsBlock paths_from;
        };

        using EntriesBlock = std::unordered_map<PointId, Entry>;

        template <typename PathType>
        bool upd_to(PointId pid, int_t level, PathType&& new_path)
        {
            bool has_updated = false;

            auto& current_path = data[level][pid].path_to;
            if (current_path.distance < new_path.distance)
            {
                has_updated = true;
                current_path = std::forward<Path>(new_path);
            }

            return has_updated;
        }

        template <typename PathType>
        bool upd_from(PointId from, PointId to, int_t level, PathType&& new_path)
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

        [[nodiscard]] bool entry_condition(const cvector<int>& p,
                                           int_t entry_index) const;

        [[nodiscard]] bool bound_condition(const cvector<int>& p,
                                           int_t entry_index) const;

        void populate_entry_from(int_t entry_index, PointId from);

        void populate();

    public:
        const ilp_task& ilpTask;

        std::vector<EntriesBlock> data;
        std::vector<cvector<int>> b_cuts;
        std::vector<int> bounds;
        int_t K;
        int_t H;

        std::unordered_map<cvector<int_t>, std::size_t, detail::VectorHash<int_t>> m_points;
    };

    ilp_solution jansen_rohwedder(const ilp_task& ilpTask);
	
} // namespace ilp

#endif // ILP_JANSEN_ROHWEDDER_HPP