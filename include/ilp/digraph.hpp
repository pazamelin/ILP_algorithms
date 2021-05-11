#ifndef ILP_DIGRAPH_HPP
#define ILP_DIGRAPH_HPP

#include "ilp_task.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adj_list_serialize.hpp>

#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <utility>
#include <functional>

namespace ilp
{
    //////////////////////////
    //   BASE GRAPH TYPES   //
    //////////////////////////

    using VerticesStorageType = boost::vecS;
    using EdgesStorageType    = boost::vecS;
    using DirectedCategory    = boost::directedS;

    using BaseGraphTraits = boost::adjacency_list_traits<
              VerticesStorageType
            , EdgesStorageType
            , DirectedCategory
            >;

    using VertexDescriptor = BaseGraphTraits::vertex_descriptor;
    using EdgeDescriptor   = BaseGraphTraits::edge_descriptor;

    struct VertexProperty
    {
        cvector<int> point;
        VertexDescriptor predecessor = { };
        int distance = std::numeric_limits<int>::max();
    };

    struct EdgeProperty
    {
        int weight;
        int column;
    };

    using BaseGraph = boost::adjacency_list<
              VerticesStorageType
            , EdgesStorageType
            , DirectedCategory
            , VertexProperty
            , EdgeProperty
            >;

    ///////////////////////
    //   ADAPTOR GRAPH   //
    ///////////////////////

    struct DigraphAdaptor
    {
    public:
        explicit DigraphAdaptor(const ilp::ilp_task& ilpTask);

        template <typename VertexPoint>
        std::pair<VertexDescriptor, bool> add_vertex(VertexPoint&& point)
        {
            auto it = m_points_set.find(point);
            if (it != m_points_set.end())
            {
                return {it->second, false};
            }

            auto vertex_descriptor = boost::add_vertex(VertexProperty{}, m_base);
            auto [new_it, is_inserted] = m_points_set.emplace(
                    std::make_pair(std::forward<VertexPoint>(point), vertex_descriptor)
            );
            m_base[vertex_descriptor].point = new_it->first;

            return {vertex_descriptor, true};
        }

        template <typename EdgePropertyType>
        std::pair<EdgeDescriptor, bool> add_edge(VertexDescriptor lhs,
                                                 VertexDescriptor rhs,
                                                 EdgePropertyType&& property
        )
        {
            return boost::add_edge(lhs, rhs, std::forward<EdgePropertyType>(property), m_base);
        }

        bool is_feasible() const noexcept;

    public:
        const ilp::ilp_task& ilpTask;

        BaseGraph m_base;
        std::unordered_map<cvector<int>, VertexDescriptor, detail::VectorHash<int>> m_points_set;
        
        VertexDescriptor start;
        VertexDescriptor end;
        bool bIsFeasible;

    private:
        virtual bool populate_condition(const cvector<int>& b,
                                        const cvector<int>& p,
                                        int bound) const = 0;

        virtual void populate_from(VertexDescriptor vertex,
                                   int bound) = 0;

        virtual void populate_graph() = 0;
    };

} // namespace ilp

#endif // ILP_DIGRAPH_HPP