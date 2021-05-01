#include "ilp/digraph.hpp"
#include "ilp/detail/debug.hpp"

namespace ilp
{
    DigraphAdaptor::DigraphAdaptor(const ilp::ilp_task& ilpTask)
        : ilpTask{ilpTask}
    { }  

    bool DigraphAdaptor::is_feasible() const noexcept
    {
        return bIsFeasible;
    }


} // namespace ilp