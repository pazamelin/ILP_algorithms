#include "ilp/jansen_rohwedder.hpp"
#include "ilp/detail/debug.hpp"

#include <iostream>
#include <stack>
#include <algorithm>

#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>

namespace ilp
{
	JRDigraph::JRDigraph(const ilp::ilp_task& ilpTask) : DigraphAdaptor{ilpTask}
	{
        detail::debug_log("building JR digraph ...");
    }

	bool JRDigraph::populate_condition(const cvector<int>& b,
                                       const cvector<int>& p,
                                       int_t bound) const
	{

	}

    void JRDigraph::populate_from(VertexDescriptor vertex,
                                  int_t bound)
    {

    }

    void JRDigraph::populate_graph()
    {

    }

}  // namespace ilp