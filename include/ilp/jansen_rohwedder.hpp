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

    bool dynamic_table_condition(const cvector<int_t>& b_cut,
                                 const cvector <int_t>& p,
                                 int_t bound);

	struct JRDigraph : public DigraphAdaptor
	{
		explicit JRDigraph(const ilp::ilp_task& ilpTask);

		bool populate_condition(const cvector<int>& b,
                                const cvector<int>& p,
                                int_t bound) const override;

        void populate_from(VertexDescriptor vertex,
                           int_t bound) override;

        void populate_graph() override;

        detail::dynamic_table_t dtable;
        std::vector<cvector<int_t>> b_cuts;
        std::vector<int_t> populate_bounds;
        int_t H = compute_H(ilpTask.A);
        int_t K = compute_K(ilpTask);
        int_t current_level = 0;
    };

    void jansen_rohwedder(const ilp_task& ilpTask);
	
} // namespace ilp

#endif // ILP_JANSEN_ROHWEDDER_HPP