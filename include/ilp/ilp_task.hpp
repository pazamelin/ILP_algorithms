#ifndef ILP_TASK_HPP
#define ILP_TASK_HPP

#include <eigen3/Eigen/Core>
#include <boost/container_hash/hash.hpp>

#include <utility>

namespace ilp
{
    using index_t = Eigen::Index;

    template <typename T>
    using matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;

    template <typename T>
    using rvector = Eigen::Matrix<T, 1, Eigen::Dynamic>;

    template <typename T>
    using cvector = Eigen::Matrix<T, Eigen::Dynamic, 1>;

    namespace detail
    {
        template <typename T>
        struct VectorHash
        {
            std::size_t operator () (const rvector<T>& row) const
            {
                std::size_t seed = 0;
                for (index_t i = 0; i != row.cols(); ++i)
                {
                    boost::hash_combine(seed, row(i, 0));
                }

                return seed;
            }

            std::size_t operator () (const cvector<T>& column) const
            {
                std::size_t seed = 0;
                for (index_t i = 0; i != column.rows(); ++i)
                {
                    boost::hash_combine(seed, column(i, 0));
                }

                return seed;
            }
        };

    } // namespace detail

    template <typename T>
    std::ostream& operator << (std::ostream& os, const rvector<T>& vec)
    {
        bool flag{false};

        os << "(";
        for (index_t col = 0; col < vec.cols(); ++col)
        {
            if (flag)
            {
                os << ", ";
            }
            flag = true;
            os << vec(col);
        }
        os << ")";
        return os;
    }

    template <typename T>
    std::ostream& operator << (std::ostream& os, const cvector<T>& vec)
    {
        bool flag{false};

        os << "(";
        for (index_t row = 0; row < vec.rows(); ++row)
        {
            if (flag)
            {
                os << ", ";
            }
            flag = true;
            os << vec(row);
        }
        os << ")^T";
        return os;
    }

    class ilp_task
    {
    public:
        ilp_task(matrix<int> A, cvector<int> b, rvector<int> c);
        ~ilp_task() = default;

        [[nodiscard]] std::size_t size_m() const;
        [[nodiscard]] std::size_t size_n() const;

    public:
        matrix<int> A;
        cvector<int> b;
        rvector<int> c;
        cvector<int> x;

        std::size_t m;
        std::size_t n;
    };

    struct ilp_solution
    {
        bool is_feasible = false;
        bool is_bounded = false;
        std::vector<int> x;
        int c_result;
    };

} // namespace ilp

#endif // ILP_TASK_HPP
