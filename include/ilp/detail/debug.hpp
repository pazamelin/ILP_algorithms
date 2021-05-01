#ifndef ILP_DETAIL_DEBUG_HPP
#define ILP_DETAIL_DEBUG_HPP

//#define ILP_DIGRAPH_DEBUG_LOG

#include <iostream>

namespace ilp::detail
{
    template <typename Message, typename Value>
    void debug_log(Message&& msg, Value&& value)
    {
#ifdef ILP_DIGRAPH_DEBUG_LOG
        std::cerr << msg << " " << value << '\n';
#endif
    }

    template <typename Message>
    void debug_log(Message&& msg)
    {
#ifdef ILP_DIGRAPH_DEBUG_LOG
        std::cerr << msg << '\n';
#endif
    }

} // namespace ilp::detail

#endif // ILP_DETAIL_DEBUG_HPP