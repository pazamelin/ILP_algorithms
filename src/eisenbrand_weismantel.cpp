#include "ilp/eisenbrand_weismantel.hpp"
#include "ilp/detail/debug.hpp"
#include "ilp/detail/bellman_ford.hpp"

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
	EWDigraph::EWDigraph(const ilp::ilp_task& ilpTask) : DigraphAdaptor{ilpTask}
	{     
	    detail::debug_log("building EW digraph ...");
	}

	bool EWDigraph::populate_condition(const cvector<int>& b,
                                       const cvector<int>& p,
                                       int_t bound) const
	{
		{
            detail::debug_log("populate_condition call:");
        }

        double dot_product_lhs = b.dot(p);
        double dot_product_rhs = b.dot(b);
        double coeff = dot_product_lhs / dot_product_rhs;
        cvector<double> b_cut = b.cast<double>() * std::min(coeff, 1.0);

        {
            detail::debug_log("     checking: ", p);
            detail::debug_log("     b offcut: ", b_cut);
        }

        cvector<double> distance = b_cut - p.cast<double>();
        auto norm = distance.lpNorm<Eigen::Infinity>();
        bool result = (norm <= bound);

        {
            detail::debug_log("     distance: ", distance);
            detail::debug_log("     bound:", bound);
            result ? detail::debug_log("     OK, populating!")
                   : detail::debug_log("     NO, out bounds.");
        }

        return result;
	}

    void EWDigraph::populate_from(VertexDescriptor vertex,
                                  int_t bound)
    {
    	const auto& point = *(this->m_base[vertex].point);
        detail::debug_log("populate_digraph on:", point);

        std::stack<VertexDescriptor> populated;

        // iterate over A's columns
        for(index_t i = 0; i != ilpTask.A.cols(); i++)
        {
            const cvector<int>& column = ilpTask.A.col(i);
            cvector<int> possible_new_point = point + column;

            if (populate_condition(ilpTask.b, possible_new_point, bound))
            {
                // try to insert and get descriptor of a new or an existing vertex
                auto [new_vertex, is_new] = this->add_vertex(std::move(possible_new_point));

                // insert the new edge to the graph
                this->add_edge(vertex,
                               new_vertex,
                               EdgeProperty{-1 * ilpTask.c(i), static_cast<int_t>(i)}
                );

                // save the vertex descriptor for later recursive call of the populate function
                if (is_new)
                {
                    populated.push(new_vertex);
                }
            }
        }

        while (!populated.empty())
        {
            // recursive call of the populate function on the new points
            auto populated_vertex = populated.top();
            populated.pop();
            populate_from(populated_vertex, bound);
        }
    }

    void EWDigraph::populate_graph()
    {
        const index_t m   = ilpTask.A.rows();
        const index_t n   = ilpTask.A.cols();
        const int_t delta = ilpTask.A.lpNorm<Eigen::Infinity>();
        const int_t bound = 2 * static_cast<int_t>(m) * delta;

        this->start = this->add_vertex(cvector<int>::Zero(n, 1)).first;
        this->m_base[start].distance = 0;

        populate_from(start, bound);

        auto b_it = m_points_set.find(ilpTask.b);
        this->bIsFeasible = (b_it != m_points_set.end());
        if (bIsFeasible)
        {
            this->end = b_it->second;
        }
    }

    ilp_solution eisenbrand_weismantel(const ilp_task& ilpTask)
	{
        const index_t m = ilpTask.A.rows();
        const index_t n = ilpTask.A.cols();

        ilp_solution result;
        result.x = std::vector<int_t>(n, 0);

        ilp::EWDigraph graph{ilpTask};
        graph.populate_graph();

        result.is_feasible = graph.is_feasible();
        if (result.is_feasible)
        {
            ilp::detail::debug_log("feasible");

            result.is_bounded = ilp::detail::bellman_ford(graph.start,
                                                          graph.m_base);

            if (result.is_bounded)
            {
                VertexDescriptor vd = graph.end;
                result.c_result = -1 * graph.m_base[vd].distance;

                while (vd != graph.start)
                {
                    const auto& vertex = graph.m_base[vd];
                    EdgeDescriptor ed = boost::edge(vertex.predecessor, vd, graph.m_base).first;
                    const auto& edge = graph.m_base[ed];
                    result.x[edge.column] += 1;

                    vd = vertex.predecessor;
                }

            }
        }

        return result;
    }

} // namespace ilp