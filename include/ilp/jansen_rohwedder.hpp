#ifndef ILP_JANSEN_ROHWEDDER_HPP
#define ILP_JANSEN_ROHWEDDER_HPP

#include "ilp_task.hpp"
#include "digraph.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adj_list_serialize.hpp>

#include <unordered_set>

namespace ilp
{
	struct JRDigraph : public DigraphAdaptor
	{
		explicit JRDigraph(const ilp::ilp_task& ilpTask);

		bool populate_condition(const cvector<int>& b,
                                const cvector<int>& p,
                                int_t bound) const override;

        void populate_from(VertexDescriptor vertex,
                           int_t bound) override;

        void populate_graph() override;
    };

	void jansen_rohwedder(const ilp_task& ilpTask);
	
} // namespace ilp

#endif // ILP_JANSEN_ROHWEDDER_HPP