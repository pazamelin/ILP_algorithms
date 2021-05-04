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

        struct Path
        {
            Path() = default;
            Path(cvector<int_t> x, int_t distance) : x{std::move(x)}, distance{distance} { };

            cvector<int_t> x;
            int_t distance = std::numeric_limits<int_t>::min();
        };

        using PathsBlock = std::unordered_map<cvector<int_t>, Path, detail::VectorHash<int_t>>;

        struct Entry
        {
            PathsBlock paths_from;
        };

        using EntriesBlock = std::unordered_map<cvector<int_t>, Entry, detail::VectorHash<int_t>>;

        template <typename PathType>
        bool upd_from(int_t entry_index,
                      const cvector<int_t>& from,
                      const cvector<int_t>& to,
                      PathType&& new_path)
        {
            bool has_updated = false;

            auto& current_path = data[entry_index][from].paths_from[to];
            if (current_path.distance < new_path.distance)
            {
                has_updated = true;
                current_path = std::forward<PathType>(new_path);
            }

            return has_updated;
        }

        [[nodiscard]] bool entry_condition(const cvector<int_t>& p,
                                           int_t entry_index) const;

        [[nodiscard]] bool bound_condition(const cvector<int_t>& p,
                                           int_t entry_index) const;

        void populate_entry_from(int_t entry_index, const cvector<int_t>& from);

        void populate();

        template <typename PointType>
        bool add_entry_point(PointType&& p, int_t entry_index)
        {
            bool has_inserted = false;
            auto p_it = this->data[entry_index].find(p);
            if (p_it == this->data[entry_index].end())
            {
                this->data[entry_index][std::forward<PointType>(p)] = Entry{ };
                has_inserted = true;
            }

            return has_inserted;
        }

    public:
        const ilp_task& ilpTask;

        std::vector<EntriesBlock> data;
        std::vector<cvector<int_t>> b_cuts;
        std::vector<int_t> bounds;
        int_t K;
        int_t H;
    };

    ilp_solution jansen_rohwedder(const ilp_task& ilpTask);
	
} // namespace ilp

#endif // ILP_JANSEN_ROHWEDDER_HPP