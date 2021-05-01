#ifndef ILP_EISENBRAND_WEISMANTEL_HPP
#define ILP_EISENBRAND_WEISMANTEL_HPP

#include "ilp_task.hpp"
#include "digraph.hpp"

namespace ilp
{
	struct EWDigraph : public DigraphAdaptor
	{
		explicit EWDigraph(const ilp::ilp_task& ilpTask);

		bool populate_condition(const cvector<int>& b,
                                const cvector<int>& p,
                                int_t bound) const override;

        void populate_from(VertexDescriptor vertex,
                           int_t bound) override;

        void populate_graph() override;
	};

    ilp_solution eisenbrand_weismantel(const ilp_task& ilpTask);

} // namespace ilp

#endif // ILP_EISENBRAND_WEISMANTEL_HPP