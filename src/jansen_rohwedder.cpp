#include "ilp/jansen_rohwedder.hpp"
#include "ilp/detail/debug.hpp"

#include <iostream>
#include <stack>
#include <algorithm>
#include <cmath>

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
        bool result = p.lpNorm<1>() <= bound;
        return result;
    }

    void JRDigraph::populate_from(VertexDescriptor vertex,
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

    void JRDigraph::populate_graph()
    {
        const index_t m   = ilpTask.A.rows();
        const index_t n   = ilpTask.A.cols();

        const int_t H = compute_H(ilpTask.A);
        const int_t K = compute_K(ilpTask);
        const int_t bound = 4 * H;

        // b' = (2^(i - K) * b) is in 'b_cuts' for every i = 0, 1, ..., K
        const std::vector<cvector<int_t>>
        b_cuts = [&]()
        {
            std::vector<cvector < int_t>>
            result;
            result.reserve(K);

            for (int_t i = 0; i < K; i++)
            {
                auto i_cut = ilpTask.b.cast<double>();
                i_cut *= std::pow(2, i - K);
                result.emplace_back(i_cut.cast<int_t>());
            }

            return result;
        }( );

        // (6/5)^i for every i = 0, 1, ..., K
        const auto populate_bounds = [K](){
            std::vector<int_t> result;
            result.reserve(K);

            for (int_t i = 0; i < K; ++i)
            {
                result.emplace_back((6/5)^i);
            }
            return result;
        }( );

        // dynamic programme init stage:


        this->start = this->add_vertex(cvector<int>::Zero(n, 1)).first;
        this->m_base[start].distance = 0;
        const cvector<int>& zero_point = *m_base[start].point;

        for(int_t i = 0; i < K + 1; i++)
        {
            // for every entry on the previous step
            // populate graph and the dynamic table
        }

    }

    int_t compute_H(const matrix <int_t>& A)
    {
        int_t result = 0;
        for (index_t i = 0; i < A.cols(); ++i)
        {
            result = std::max(result, A.col(i).lpNorm<1>());
        }

        detail::debug_log("     H: ", result);
        return result;
    }

    int_t compute_K(const ilp_task& ilpTask)
    {
        auto m = static_cast<double>(ilpTask.A.rows());
        auto n = static_cast<double>(ilpTask.A.cols());
        auto delta = static_cast<double>(ilpTask.A.lpNorm<Eigen::Infinity>());

        double a = m * std::log2(m);
        double b = m * std::log2(delta + ilpTask.b.lpNorm<Eigen::Infinity>());
        auto result = static_cast<int_t>(a + b);

        detail::debug_log("     K: ", result);
        return result;
    }

    bool dynamic_table_condition(const cvector<int_t>& b_cut,
                                 const cvector <int_t>& p,
                                 int_t bound)
    {
        cvector<int_t> distance = b_cut - p;
        int_t norm = distance.lpNorm<Eigen::Infinity>();
        bool result = (norm <= bound);

        return result;
    }

    void jansen_rohwedder(const ilp_task& ilpTask)
    {
        std::vector<std::vector<std::pair<cvector<int_t>, int_t>>> dynamic_table;
    }

}  // namespace ilp