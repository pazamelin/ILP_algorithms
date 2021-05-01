#ifndef ILP_BELLMAN_FORD_HPP
#define ILP_BELLMAN_FORD_HPP

#include <ilp/ilp_task.hpp>
#include <ilp/digraph.hpp>

namespace ilp::detail
{
    template <typename Edge,
              typename Graph,
              typename DistanceCmp = std::less<int_t>>
    bool relax(Edge edge_descriptor,
               Graph& graph,
               DistanceCmp dcmp = { }
    )
    {
        bool has_relaxed = false;

        auto source_descriptor = boost::source(edge_descriptor, graph);
        auto target_descriptor = boost::target(edge_descriptor, graph);

        auto& source = graph[source_descriptor];
        auto& target = graph[target_descriptor];
        auto& edge = graph[edge_descriptor];

        if ( dcmp(source.distance + edge.weight, target.distance) )
        {
            target.distance = source.distance + edge.weight;
            target.predecessor = source_descriptor;
            has_relaxed = true;
        }

        return has_relaxed;
    }

    template <typename Vertex,
              typename Graph,
              typename DistanceCmp = std::less<int_t>>
    bool bellman_ford(Vertex src,
                      Graph& graph,
                      DistanceCmp dcmp = { })
    {
        // assuming that vertices and weights are already initialized
        auto edge_range = boost::edges(graph);

        for (std::size_t i = 0; i < graph.vertex_set().size(); i++)
        {
            // try to relax every edge
            for (auto e_it = edge_range.first; e_it != edge_range.second; ++e_it)
            {
                relax(*e_it, graph, dcmp);
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

