#include "ilp/eisenbrand_weismantel.hpp"
#include "ilp/detail/debug.hpp"
#include "ilp/detail/bellman_ford.hpp"

#include <iostream>
#include <stack>
#include <algorithm>

#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/named_function_params.hpp>

// original boost header:
//#include <boost/graph/bellman_ford_shortest_paths.hpp>

// modified boost header:
//#include "ilp/detail/boost/bellman_ford_shortest_paths.hpp"

namespace ilp
{
    EWDigraph::EWDigraph(const ilp::ilp_task& ilpTask) : DigraphAdaptor{ilpTask}
    {
        detail::debug_log("building EW digraph ...");
    }

    bool EWDigraph::populate_condition(const cvector<int>& b,
                                       const cvector<int>& p,
                                       int bound) const
    {
        double dot_product_lhs = b.dot(p);
        double dot_product_rhs = b.dot(b);
        double coeff = dot_product_lhs / dot_product_rhs;
        if (coeff > 1.0)
        {
            coeff = 1.0;
        }

        for (int i = 0; i < ilpTask.A.rows(); ++i)
        {
            auto b_val = static_cast<double>(b(i, 0));
            double b_val_cut = b_val * coeff;
            double p_val = p(i, 0);
            if (std::abs(p_val - b_val_cut) > bound)
            {
                return false;
            }
        }

        return true;
    }

    void EWDigraph::populate_from(VertexDescriptor vertex,
                                  int bound)
    {
        std::stack<VertexDescriptor> populated;
        populated.push(vertex);

        while (!populated.empty())
        {
            auto current_vertex = populated.top();
            auto current_point = this->m_base[current_vertex].point;
            populated.pop();

            // iterate over A's columns
            for (index_t i = 0; i != ilpTask.A.cols(); i++)
            {
                const cvector<int>& column = ilpTask.A.col(i);
                current_point += column;

                if (populate_condition(ilpTask.b, current_point, bound))
                {
                    // try to insert and get descriptor of a new or an existing vertex
                    auto[new_vertex, is_new] = this->add_vertex(current_point);

                    if (new_vertex == current_vertex)
                    {
                        continue;
                    }

                    // insert the new edge to the graph
                    this->add_edge(current_vertex,
                                   new_vertex,
                                   EdgeProperty{-1 * ilpTask.c(i), static_cast<int>(i)}
                    );

                    // save the vertex descriptor for later recursive call of the populate function
                    if (is_new)
                    {
                        populated.push(new_vertex);
                    }
                }

                current_point -= column;
            }
        }
    }

    void EWDigraph::populate_graph()
    {
        const index_t m = ilpTask.A.rows();
        const index_t n = ilpTask.A.cols();
        const int delta = ilpTask.A.lpNorm<Eigen::Infinity>();
        const int bound = 2 * static_cast<int>(m) * delta;

        this->start = this->add_vertex(cvector<int>::Zero(m, 1)).first;
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
        result.x = cvector<int>::Zero(n, 1);

        ilp::EWDigraph graph{ilpTask};

        {
            graph.populate_graph();
        }

        result.is_feasible = graph.is_feasible();
        if (result.is_feasible)
        {
            ilp::detail::debug_log("feasible");

/*            {
                result.is_bounded = bellman_ford_shortest_paths(
                        graph.m_base,
                        num_vertices(graph.m_base),
                        predecessor_map(get(&VertexProperty::predecessor, graph.m_base))
                                .distance_map(get(&VertexProperty::distance, graph.m_base))
                                .weight_map(get(&EdgeProperty::weight, graph.m_base))
                );
            }*/

            {
                result.is_bounded = ilp::detail::bellman_ford(graph.start,
                                                              graph.m_base);
            }

            if (result.is_bounded)
            {
                VertexDescriptor vd = graph.end;
                result.c_result = -1 * graph.m_base[vd].distance;

                while (vd != graph.start)
                {
                    const auto& vertex = graph.m_base[vd];
                    EdgeDescriptor ed = boost::edge(vertex.predecessor, vd, graph.m_base).first;
                    const auto& edge = graph.m_base[ed];
                    result.x(edge.column, 0) += 1;

                    vd = vertex.predecessor;
                }

            }
        }

        return result;
    }

} // namespace ilp
