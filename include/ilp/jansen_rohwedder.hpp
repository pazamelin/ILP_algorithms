#ifndef ILP_JANSEN_ROHWEDDER_HPP
#define ILP_JANSEN_ROHWEDDER_HPP

#include "ilp_task.hpp"
#include "digraph.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adj_list_serialize.hpp>

#include <unordered_set>

namespace ilp
{
    struct dynamic_table_t;

	struct JRDigraph : public DigraphAdaptor
	{
		explicit JRDigraph(const ilp::ilp_task& ilpTask);

		bool populate_condition(const cvector<int>& b,
                                const cvector<int>& p,
                                int_t bound) const override;

        void populate_from(VertexDescriptor vertex,
                           int_t bound) override;

        void populate_graph() override;

        dynamic_table_t table;
    };

    int_t compute_H(const matrix<int_t>& A);

    int_t compute_K(const ilp_task& ilpTask);

    struct dynamic_table_t
    {   
        struct path_t
        {
            cvector<int_t> x;
            int_t distance;
        }

        using paths_block = std::onordered_set<VertexDescriptor, path_t>

        struct entry_t
        {
            path_t path_to;
            paths_block paths_from;
        }

        using entries_block = std::unordered_map<VertexDescriptor, entry_t>;

        template <typename Path>
        bool upd_to(VertexDescriptor v, int_t level, Path&& new_path)
        {
            bool has_updated = false;

            auto& current_path = data[level][v].path_to;
            if (current_path.distance < new_paht.distance)
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
            if (current_path.distance < new_paht.distance)
            {
                has_updated = true;
                current_path = std::forward<Path>(new_path);
            }

            return has_updated;
        }

        std::vector<entries_block> data;

    }

    void jansen_rohwedder(const ilp_task& ilpTask);
	
} // namespace ilp

#endif // ILP_JANSEN_ROHWEDDER_HPP