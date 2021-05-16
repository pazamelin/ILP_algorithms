#ifndef ILP_BELLMAN_FORD_HPP
#define ILP_BELLMAN_FORD_HPP

#include <ilp/ilp_task.hpp>
#include <ilp/digraph.hpp>

#include <tbb/tbb.h>

namespace ilp::detail
{
    template <typename Edge,
            typename Graph,
            typename DistanceCmp = std::less<int>>
    bool relax(Edge edge_descriptor,
               Graph& graph,
               DistanceCmp dcmp = {}
    )
    {
        bool has_relaxed = false;

        auto source_descriptor = boost::source(edge_descriptor, graph);
        auto target_descriptor = boost::target(edge_descriptor, graph);

        auto& source = graph[source_descriptor];
        auto& target = graph[target_descriptor];
        auto& edge = graph[edge_descriptor];

        if (dcmp(source.distance + edge.weight, target.distance))
        {
            target.distance = source.distance + edge.weight;
            target.predecessor = source_descriptor;
            has_relaxed = true;
        }

        return has_relaxed;
    }

    template <typename Vertex,
            typename Graph,
            typename DistanceCmp = std::less<int>>
    bool bellman_ford(Vertex src,
                      Graph& graph,
                      DistanceCmp dcmp = {})
    {
        // assuming that vertices and weights are already initialized
        const auto& edge_range = boost::edges(graph);

        const auto num_v = num_vertices(graph);

        for (int i = 0; i < num_v; i++)
        {
            bool at_least_one_edge_relaxed = false;

            tbb::parallel_for(
                    tbb::blocked_range<VertexDescriptor>{0, num_v},
                    [&](const tbb::blocked_range<VertexDescriptor>& r)
                    {
                        for (VertexDescriptor v = r.begin(); v < r.end(); v++)
                        {
                            typename boost::graph_traits<BaseGraph>::out_edge_iterator ei, ei_end;
                            for (boost::tie(ei, ei_end) = out_edges(v, graph); ei != ei_end; ++ei)
                            {
                                // try to relax the edge
                                if (relax(*ei, graph, dcmp))
                                {
                                    at_least_one_edge_relaxed = true;
                                }
                            }
                        }
                    }
            );

            if (!at_least_one_edge_relaxed)
            {
                break;
            }
        }

        // detect negative weights cycles
        for (auto e_it = edge_range.first; e_it != edge_range.second; ++e_it)
        {
            if (relax(*e_it, graph, dcmp))
            {
                return false;
            }
        }

        return true;
    }

} // namespace ilp::detail

#endif // ILP_BELLMAN_FORD_HPP

