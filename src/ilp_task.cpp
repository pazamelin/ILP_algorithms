#include "ilp/ilp_task.hpp"

namespace ilp
{
    ilp_task::ilp_task(matrix<int_t> A, cvector<int_t> b, rvector<int_t> c)
        : A{std::move(A)}
        , b{std::move(b)}
        , c{std::move(c)}
        , m{static_cast<size_t>(A.rows())}
        , n{static_cast<size_t>(A.cols())}
    { }

    [[nodiscard]] std::size_t ilp_task::size_m() const
    {
        return m;
    }

    [[nodiscard]] std::size_t ilp_task::size_n() const
    {
        return n;
    }

} // namespace ilp
